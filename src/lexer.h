#ifndef H_LEXER
#define H_LEXER

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


typedef enum TK_TYPE {
    NEWLINE,
    VALUE,
    ASSIGNMENT,
    OUTPUT
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


Lexer *lex(const char *data);
void lex_num(const char *data, Lexer *lexer);

Lexer *lexer_init(void);
void lexer_free(Lexer *lexer);
void lexer_add(Lexer *lexer, TK_TYPE type, size_t value);

#endif
