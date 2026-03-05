#ifndef H_PARSER
#define H_PARSER

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "lexer.h"
#define IMPLEMENT_STACK
#include "stacks.h"

IMPLEMENT_TYPE_STACK(TK_TYPE, Op_Stack)

typedef enum ND_TYPE {
    ND_OUTPUT,
    ND_OUTPUT_INT,
    
    ND_VALUE,
    ND_EXPRESSION,

    ND_ADDRESS,

    ND_NAND,
    ND_ASSIGNMENT
} ND_TYPE;


typedef struct Node {
    ND_TYPE type;
    size_t value;
    struct Node *left;
    struct Node *right;
} Node;

IMPLEMENT_TYPE_STACK(Node *, ND_Stack)

typedef struct {
    size_t len;
    size_t cap;
    Node *statements;
} AST;

typedef struct {
    size_t ptr;
    AST ast;
} Parser;


Parser *parse(const Token *tokens, size_t len);
Node *parse_expr(const Token *tokens, size_t len, Parser *parser);
void parse_op(const Token *tokens, size_t len, Parser *parser, TK_TYPE);
void parse_keyword(const Token *tokens, size_t len, Parser *parser);

Parser *parser_init(void);
void parser_free(Parser *parser);
void parser_push(Parser *parser, Node *node);

Node *node_init(ND_TYPE type, size_t value);
void node_free(Node *node);

#endif
