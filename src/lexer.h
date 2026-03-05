#ifndef H_LEXER
#define H_LEXER

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


typedef enum TK_TYPE {
    TK_VALUE,

    TK_LBRACKET,
    TK_LSBRACKET,
    TK_NAND,
    TK_ASSIGNMENT,
    TK_RSBRACKET,
    TK_RBRACKET,


    TK_KEYWORD,
    TK_OUTPUT,
    TK_OUTPUT_INT,
    TK_NEWLINE
} TK_TYPE;


typedef struct {
    TK_TYPE type;
    size_t value;
} Token;

typedef struct {
    size_t len;
    size_t cap;
    Token *tokens;
    size_t ptr;
} Lexer;


Lexer *lex(const char *program, size_t len);
void lex_num(const char *program, size_t len, Lexer *lexer);

Lexer *lexer_init(void);
void lexer_free(Lexer *lexer);
void lexer_push(Lexer *lexer, TK_TYPE type, size_t value);

#endif
