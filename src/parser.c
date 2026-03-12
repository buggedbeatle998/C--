#include "parser.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define IMPLEMENT_STACK
#include "stacks.h"
#include "comp_error.h"


Parser *parse(Lexer *lexer) {
    Parser *parser = parser_init();

    Node *temp = parse_scope(lexer, parser, true);
    parser->global_scope = *temp;
    free(temp);

    return parser;
}


Node *parse_statement(Lexer *lexer, Parser *parser) {
    const Token *tokens =  lexer->tokens;
    Node *temp;

    Token val = tokens[parser->ptr];
    switch (val.type) {
        case TK_KEYWORD:
            temp = parse_keyword(lexer, parser);
            return temp;

        case TK_OUTPUT:
            temp = node_init(ND_OUTPUT, 0);
            ++parser->ptr;
            temp->left = parse_expr(lexer, parser);
            break;

        case TK_OUTPUT_INT:
            temp = node_init(ND_OUTPUT_INT, 0);
            ++parser->ptr;
            temp->left = parse_expr(lexer, parser);
            break;

        case TK_LBRACKET:
        case TK_LSBRACKET:
        case TK_VALUE:
            temp = node_init(ND_EXPRESSION, 0);
            temp->left = parse_expr(lexer, parser);
            break;

        case TK_LCBRACKET:
            ++parser->ptr;
            temp = parse_scope(lexer, parser, false);
            return temp;

        default:
            parser_free(parser);
            lexer_free(lexer);
            comp_error("Parser", "Unexpected Token!", val.line, val.col);
            break;
    }


    if (tokens[parser->ptr++].type != TK_NEWLINE) {
        val = tokens[parser->ptr - 1];
        parser_free(parser);
        lexer_free(lexer);
        comp_error("Parser", "Expected Token! \";\"!", val.line, val.col);
    }

    return temp;
}


Node *parse_scope(Lexer *lexer, Parser *parser, bool global) {
    const Token *tokens =  lexer->tokens;
    size_t len = lexer->len;
    AST ast = {0, 16, malloc(sizeof(Node) * 16)};
    parser_ast_push(parser, ast.statements);
    Token val;
    Node *temp;

    while (parser->ptr < len) {
        val = tokens[parser->ptr];
        if (val.type == TK_NEWLINE) {
            ++parser->ptr;
        } else if (val.type == TK_RCBRACKET) {
            if (global) {
                parser_free(parser);
                lexer_free(lexer);
                comp_error("Parser", "Unexpected Token!", val.line, val.col);
            }
            ++parser->ptr;
            temp = node_init(ND_SCOPE, ast.len);
            temp->left = ast.statements;
            parser_ast_pop(parser);
            return temp;

        } else if (val.type == TK_EOF) {
            if (!global) {
                parser_free(parser);
                lexer_free(lexer);
                comp_error("Parser", "Expected Token \"}\"!", 0, 0);
            }
            temp = node_init(ND_SCOPE, ast.len);
            temp->left = ast.statements;
            parser_ast_pop(parser);
            return temp;
        } else {
            ast_push(&ast, parse_statement(lexer, parser));
        }
    }
    
    parser_free(parser);
    lexer_free(lexer);
    comp_error("Parser", "Fatal EOF error!", -1, -1);
    // Shut up warning!
    return NULL;
}


#define UNEX_TK_EXPR Op_Stack_free(opstack); \
                     ND_Stack_free(nstack); \
                     parser_free(parser); \
                     lexer_free(lexer); \
                     comp_error("Parser", "Unexpected Token!", val.line, val.col);

IMPLEMENT_TYPE_STACK(Token, Op_Stack)
IMPLEMENT_TYPE_STACK(Node *, ND_Stack)

Node *parse_expr(Lexer *lexer, Parser *parser) {
    const Token *tokens =  lexer->tokens;
    size_t len = lexer->len;
    Token val = tokens[parser->ptr];
    TK_TYPE prev = TK_LBRACKET;
    if (val.type > TK_LSBRACKET) {
        parser_free(parser);
        lexer_free(lexer);
        comp_error("Parser", "Unexpected Token!", val.line, val.col);
    }
    ND_Stack nstack = ND_Stack_init(256);
    Op_Stack opstack = Op_Stack_init(256);

    while (parser->ptr < len) {
        val = tokens[parser->ptr++];
        if (val.type == TK_VALUE) {
            if (prev < TK_LBRACKET || prev > TK_ASSIGNMENT) {
                UNEX_TK_EXPR
            }
            ND_Stack_push(nstack, node_init(ND_VALUE, val.value));
        
        } else if (TK_LBRACKET <= val.type && val.type <= TK_LSBRACKET) {
            if (prev < TK_LBRACKET || prev > TK_ASSIGNMENT) {
                UNEX_TK_EXPR
            }
            Op_Stack_push(opstack, val);
        
        } else if (TK_LCBRACKET < val.type && val.type < TK_RCBRACKET) {
            if (prev != TK_VALUE && prev != TK_RSBRACKET && prev != TK_RBRACKET) {
                UNEX_TK_EXPR
            }
            while (!Op_Stack_is_empty(opstack) && Op_Stack_peek(opstack).type > TK_LSBRACKET
                    && (Op_Stack_peek(opstack).type < val.type
                        || (val.type != TK_ASSIGNMENT && Op_Stack_peek(opstack).type == val.type))) {
                Node *temp = node_init(ND_NAND + Op_Stack_peekpop(opstack).type - TK_NAND, 0);
                temp->right = ND_Stack_peekpop(nstack);
                temp->left = ND_Stack_peekpop(nstack);
                ND_Stack_push(nstack, temp);
            }
            Op_Stack_push(opstack, val);
        
        } else if (TK_RSBRACKET <= val.type && val.type <= TK_RBRACKET) {
            if (prev != TK_VALUE && prev != TK_RSBRACKET && prev != TK_RBRACKET) {
                UNEX_TK_EXPR
            }
            if (Op_Stack_is_empty(opstack))
                break;
            while (!Op_Stack_is_empty(opstack) && Op_Stack_peek(opstack).type > TK_LSBRACKET) {
                Node *temp = node_init(ND_NAND + Op_Stack_peekpop(opstack).type - TK_NAND, 0);
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
                if (ND_Stack_size(nstack) < 2) {
                    UNEX_TK_EXPR
                }
                Node *temp = node_init(ND_NAND + Op_Stack_peekpop(opstack).type - TK_NAND, 0);
                temp->right = ND_Stack_peekpop(nstack);
                temp->left = ND_Stack_peekpop(nstack);
                ND_Stack_push(nstack, temp);
            }
            break;
        }
        prev = val.type;
    }
    --parser->ptr;
    
    Node *temp = ND_Stack_peek(nstack);

    Op_Stack_free(opstack);
    ND_Stack_free(nstack);

    return temp;
}


Node *parse_keyword(Lexer *lexer, Parser *parser) {
    const Token *tokens = lexer->tokens;
    Node *node;
    const char *kw = tokens[parser->ptr++].str_val;
    
    if (strcmp(kw, "if") == 0) {
        node = node_init_comp(ND_IF);
    } else if (strcmp(kw, "while") == 0) {
        node = node_init_comp(ND_WHILE);
    } else {
        Token val = tokens[parser->ptr - 1];
        parser_free(parser);
        lexer_free(lexer);
        comp_error("Parser", "Unrecognised keyword!", val.line, val.col);
    }

    if (tokens[parser->ptr++].type != TK_LBRACKET) {
        Token val = tokens[--parser->ptr];
        parser_free(parser);
        lexer_free(lexer);
        comp_error("Parser", "Expected Token \"(\"!", val.line, val.col);
    }
    
    node->comp = parse_expr(lexer, parser);

    if (tokens[parser->ptr++].type != TK_RBRACKET) {
        node_free(node);
        free(node);
        Token val = tokens[parser->ptr - 1];
        parser_free(parser);
        lexer_free(lexer);
        comp_error("Parser", "Expected Token \")\"!", val.line, val.col);
    }
    
    if (tokens[parser->ptr].type != TK_NEWLINE)
        node->left = parse_statement(lexer, parser);
    else
        ++parser->ptr;
        

    return node;
}


Parser *parser_init(void) {
    Parser *parser = malloc(sizeof(Parser));
    parser->ptr = 0;
    parser->len = 0;
    parser->cap = 16;
    parser->AST_stack = malloc(sizeof(Node) * parser->cap);

    return parser;
}


void parser_free(Parser *parser) {
    if (!parser->len)
        node_free(&parser->global_scope);
    else
        parser_ast_free(parser);
    free(parser->AST_stack);
    free(parser);
}


void parser_ast_push(Parser *parser, Node *ast) {
    if (parser->len >= parser->cap) {
        parser->cap <<= 1;
        parser->AST_stack = realloc(parser->AST_stack, sizeof(Node) * parser->cap);
    }

    parser->AST_stack[parser->len++] = ast;
}


void parser_ast_pop(Parser *parser) {
    --parser->len;
}


void parser_ast_free(Parser *parser) {
    while (parser->len) {
        Node temp = (Node){ND_SCOPE, 0, parser->AST_stack[--parser->len], NULL};
        node_free(&temp);
    }
}


void ast_init(AST *ast) {
    ast->len = 0;
    ast->cap = 64;
    ast->statements = malloc(sizeof(Node) * ast->cap);
}


void ast_push(AST *ast, Node *node) {
    if (ast->len >= ast->cap) {
        ast->cap <<= 1;
        ast->statements = realloc(ast->statements, sizeof(Node) * ast->cap);
    }

    ast->statements[ast->len++] = *node;
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


Node *node_init_comp(ND_TYPE type) {
    Node *node = malloc(sizeof(Node));
    node->type = type;
    node->comp = NULL;
    node->left = NULL;
    node->right = NULL;

    return node;
}


void node_free(Node *node) {
    if (node->type == ND_SCOPE) {
        for (size_t i = 0; i < node->value; ++i) {
            node_free(node->left + i);
        }
        free(node->left);
    } else {
        if ((node->type == ND_IF || node->type == ND_WHILE) && node->comp != NULL)
            node_free(node->comp);
        if (node->left != NULL) {
            node_free(node->left);
            free(node->left);
        }
        if (node->right != NULL) {
            node_free(node->right);
            free(node->right);
        }
    }
}
