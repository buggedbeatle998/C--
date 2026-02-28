#include "parser.h"
#include <stdio.h>

Parser *parse(const Token *tokens, size_t len) {
    Parser *parser = parser_init();
    Token val;

    while (parser->ptr < len) {
        val = tokens[parser->ptr];
        switch (val.type) {
            case TK_NEWLINE:
                ++parser->ptr;
                break;

            case TK_KEYWORD:
                //parse_keyword(tokens, len, parser);
                break;

            case TK_LBRACKET:
            case TK_LSBRACKET:
            case TK_NOT:
            case TK_VALUE:
                parse_expr(tokens, len, parser);
                break;

            default:
                ++parser->ptr;
                break;
        }
    }

    return parser;
}


void parse_expr(const Token *tokens, size_t len, Parser *parser) {
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
                    && Op_Stack_peek(opstack) <= val.type) {
                Node *temp = node_init(ND_NOT + Op_Stack_peek(opstack) - TK_NOT, 0);
                if (Op_Stack_peekpop(opstack) > TK_NOT)
                    temp->right = ND_Stack_peekpop(nstack);
                temp->left = ND_Stack_peekpop(nstack);
                ND_Stack_push(nstack, temp);
            }
            Op_Stack_push(opstack, val.type);
        
        } else if (TK_RSBRACKET <= val.type && val.type <= TK_RBRACKET) {
            if (Op_Stack_is_empty(opstack))
                break;
            while (!Op_Stack_is_empty(opstack) && Op_Stack_peek(opstack) > TK_LSBRACKET) {
                Node *temp = node_init(ND_NOT + Op_Stack_peek(opstack) - TK_NOT, 0);
                if (Op_Stack_peekpop(opstack) > TK_NOT)
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
                Node *temp = node_init(ND_NOT + Op_Stack_peek(opstack) - TK_NOT, 0);
                if (Op_Stack_peekpop(opstack) > TK_NOT)
                    temp->right = ND_Stack_peekpop(nstack);
                temp->left = ND_Stack_peekpop(nstack);
                ND_Stack_push(nstack, temp);
            }
        }
    }
    --parser->ptr;
    
    parser_push(parser, ND_Stack_peek(nstack));

    Op_Stack_free(opstack);
    ND_Stack_free(nstack);
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
    free(parser->ast.statements);
    free(parser);
}


void parser_push(Parser *parser, Node *node) {
    if (parser->ast.len >= parser->ast.cap) {
        parser->ast.cap <<= 1;
        parser->ast.statements = realloc(parser->ast.statements, parser->ast.cap);
    }

    parser->ast.statements[parser->ast.len++] = *node;
    free(node);
}


Node *node_init(ND_TYPE type, size_t value) {
    Node *node = malloc(sizeof(Node));
    node->type = type;
    node->value = value;
    node->left = 0;
    node->right = 0;

    return node;
}
