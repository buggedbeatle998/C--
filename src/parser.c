#include "parser.h"
#include <stdio.h>


Parser *parse(const Token *tokens, size_t len) {
    Parser *parser = parser_init();
    Token val;
    Node *temp;

    while (parser->ptr < len) {
        val = tokens[parser->ptr];
        switch (val.type) {
            case TK_NEWLINE:
                ++parser->ptr;
                break;

            case TK_KEYWORD:
                //parse_keyword(tokens, len, parser);
                break;

            case TK_OUTPUT:
                temp = node_init(ND_OUTPUT, 0);
                ++parser->ptr;
                temp->left = parse_expr(tokens, len, parser);
                parser_push(parser, temp);
                break;

            case TK_OUTPUT_INT:
                temp = node_init(ND_OUTPUT_INT, 0);
                ++parser->ptr;
                temp->left = parse_expr(tokens, len, parser);
                parser_push(parser, temp);
                break;

            case TK_LBRACKET:
            case TK_LSBRACKET:
            case TK_VALUE:
                temp = node_init(ND_EXPRESSION, 0);
                temp->left = parse_expr(tokens, len, parser);;
                parser_push(parser, temp);
                break;

            default:
                ++parser->ptr;
                break;
        }
    }

    return parser;
}


Node *parse_expr(const Token *tokens, size_t len, Parser *parser) {
    ND_Stack nstack = ND_Stack_init(256);
    Op_Stack opstack = Op_Stack_init(256);
    Token val;

    while (parser->ptr < len) {
        val = tokens[parser->ptr++];
        if (val.type == TK_VALUE) {
            ND_Stack_push(nstack, node_init(ND_VALUE, val.value));
        
        } else if (TK_LBRACKET <= val.type && val.type <= TK_LSBRACKET) {
            Op_Stack_push(opstack, val.type);
        
        } else if (TK_LSBRACKET < val.type && val.type < TK_RSBRACKET) {
            while (!Op_Stack_is_empty(opstack) && Op_Stack_peek(opstack) > TK_LSBRACKET
                    && (Op_Stack_peek(opstack) < val.type
                        || (val.type != TK_ASSIGNMENT && Op_Stack_peek(opstack) == val.type))) {
                //printf("1\n");
                Node *temp = node_init(ND_NAND + Op_Stack_peekpop(opstack) - TK_NAND, 0);
                temp->right = ND_Stack_peekpop(nstack);
                temp->left = ND_Stack_peekpop(nstack);
                ND_Stack_push(nstack, temp);
            }
            Op_Stack_push(opstack, val.type);
        
        } else if (TK_RSBRACKET <= val.type && val.type <= TK_RBRACKET) {
            if (Op_Stack_is_empty(opstack))
                break;
            while (!Op_Stack_is_empty(opstack) && Op_Stack_peek(opstack) > TK_LSBRACKET) {
                //printf("2\n");
                Node *temp = node_init(ND_NAND + Op_Stack_peekpop(opstack) - TK_NAND, 0);
                temp->right = ND_Stack_peekpop(nstack);
                temp->left = ND_Stack_peekpop(nstack);
                ND_Stack_push(nstack, temp);
            }
            Op_Stack_pop(opstack);
            if (val.type == TK_RSBRACKET) {
                Node *temp = node_init(ND_ADDRESS, 0);
                temp->left = ND_Stack_peekpop(nstack);
                ND_Stack_push(nstack, temp);
            }
        
        
        } else {
            while (!Op_Stack_is_empty(opstack)) {
                //printf("3\n");
                Node *temp = node_init(ND_NAND + Op_Stack_peekpop(opstack) - TK_NAND, 0);
                temp->right = ND_Stack_peekpop(nstack);
                temp->left = ND_Stack_peekpop(nstack);
                ND_Stack_push(nstack, temp);
            }
            break;
        }
    }
    --parser->ptr;
    
                //printf("4\n");
    Node *temp = ND_Stack_peek(nstack);

    Op_Stack_free(opstack);
    ND_Stack_free(nstack);

    return temp;
}


void parse_keyword(const Token *tokens, size_t len, Parser *parser);


Parser *parser_init(void) {
    Parser *parser = malloc(sizeof(Parser));
    parser->ptr = 0;
    parser->ast.len = 0;
    parser->ast.cap = 64;
    parser->ast.statements = malloc(sizeof(Node) * parser->ast.cap);

    return parser;
}


void parser_free(Parser *parser) {
    for (size_t i = 0; i < parser->ast.len; ++i) {
        if (parser->ast.statements[i].left != NULL)
            node_free(parser->ast.statements[i].left);
        if (parser->ast.statements[i].right != NULL)
            node_free(parser->ast.statements[i].right);
    }
    free(parser->ast.statements);
    free(parser);
}


void parser_push(Parser *parser, Node *node) {
    if (parser->ast.len >= parser->ast.cap) {
        parser->ast.cap <<= 1;
        parser->ast.statements = realloc(parser->ast.statements, sizeof(Node) * parser->ast.cap);
    }

    parser->ast.statements[parser->ast.len++] = *node;
    free(node);
}


Node *node_init(ND_TYPE type, size_t value) {
    Node *node = malloc(sizeof(Node));
    node->type = type;
    node->value = value;
    node->left = NULL;
    node->right = NULL;

    return node;
}


void node_free(Node *node) {
    if (node->left != NULL)
        node_free(node->left);
    if (node->right != NULL)
        node_free(node->right);
    free(node);
}
