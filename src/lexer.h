#ifndef H_LEXER
#define H_LEXER

#include <stdlib.h>


typedef enum TK_TYPE {
    TK_VALUE,

    TK_LBRACKET,
    TK_LSBRACKET,
    TK_LCBRACKET,
    TK_NAND,
    TK_ASSIGNMENT,
    TK_RCBRACKET,
    TK_RSBRACKET,
    TK_RBRACKET,

    TK_KEYWORD,
    TK_OUTPUT,
    TK_OUTPUT_INT,
    TK_NEWLINE,
    TK_EOF
} TK_TYPE;


typedef struct {
    TK_TYPE type;
    int line;
    int col;
    union {
        size_t value;
        char *str_val;
    };
} Token;

typedef struct Lexer {
    size_t len;
    size_t cap;
    Token *tokens;
    size_t ptr;
    size_t line_ptr;
    int line;
    int col;
} Lexer;


Lexer *lex(const char *program, size_t len);
void lex_num(const char *program, size_t len, Lexer *lexer);
void lex_keyword(const char *program, size_t len, Lexer *lexer);

Lexer *lexer_init(void);
void lexer_free(Lexer *lexer);
void lexer_next(Lexer *lexer);
void lexer_newline(Lexer *lexer);
void lexer_push(Lexer *lexer, TK_TYPE type, size_t value);
void lexer_push_str(Lexer *lexer, TK_TYPE type, char *value);

#endif
