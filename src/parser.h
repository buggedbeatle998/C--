#ifndef H_PARSER
#define H_PARSER

#include <stdlib.h>
#include <stdbool.h>

#include "lexer.h"


typedef enum ND_TYPE {
    ND_WHILE,
    ND_IF,

    ND_OUTPUT,
    ND_OUTPUT_INT,

    ND_SCOPE,
    
    ND_VALUE,
    ND_EXPRESSION,

    ND_ADDRESS,

    ND_NAND,
    ND_ASSIGNMENT
} ND_TYPE;


typedef struct Node {
    ND_TYPE type;
    union {
        size_t value;
        struct Node *comp;
    };
    struct Node *left;
    struct Node *right;
} Node;


typedef struct {
    size_t len;
    size_t cap;
    Node *statements;
} AST;

typedef struct {
    size_t ptr;
    Node global_scope;
    size_t len;
    size_t cap;
    Node **AST_stack;
} Parser;


Parser *parse     (Lexer *lexer);
Node *parse_statement(Lexer *lexer, Parser *parser);
Node *parse_scope (Lexer *lexer, Parser *parser, bool global);
Node *parse_expr  (Lexer *lexer, Parser *parser);
Node *parse_keyword(Lexer *lexer, Parser *parser);

Parser *parser_init(void);
void parser_free(Parser *parser);
void parser_ast_push(Parser *parser, Node *ast);
void parser_ast_pop(Parser *parser);
void parser_ast_free(Parser *parser);

void ast_init(AST *ast);
void ast_push(AST *ast, Node *node);

Node *node_init(ND_TYPE type, size_t value);
Node *node_init_comp(ND_TYPE type);
void node_free(Node *node);

#endif
