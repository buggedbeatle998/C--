#include "lexer.h"


Lexer *lex(const char *data) {
    Lexer *lexer = lexer_init();
    char val;

    while ((val = data[lexer->ptr]) != '\0') {
        switch (val) {
            case ' ':
            case '\n':
            case '\t':
                // Whitespace is ignored
                break;

            case ';':
                lexer_add(lexer, NEWLINE, 0);
                break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                lex_num(data, lexer);
                break;

            case '=':
                lexer_add(lexer, ASSIGNMENT, 0);
                break;

            case '>':
                lexer_add(lexer, OUTPUT, 0);
                break;

            default:
                break;
        }
        ++lexer->ptr;
    }

    return lexer;
}


void lex_num(const char *data, Lexer *lexer) {
    size_t total = 0;
    char val;
    
    while (true) {
        val = data[lexer->ptr];
        if (val < '0' || val > '9')
            break;
        
        total *= 10;
        total += val - '0';
        ++lexer->ptr;
    }
    --lexer->ptr;

    lexer_add(lexer, VALUE, total);
}


Lexer *lexer_init(void) {
    Lexer *lexer = malloc(sizeof(Lexer));
    lexer->len = 0;
    lexer->cap = 64;
    lexer->tokens = malloc(sizeof(Token) * lexer->cap);
    lexer->ptr = 0;

    return lexer;
}


void lexer_free(Lexer *lexer) {
    free(lexer->tokens);
    free(lexer);
}


void lexer_add(Lexer *lexer, TK_TYPE type, size_t value) {
    if (lexer->len >= lexer->cap) {
        lexer->cap <<= 1;
        lexer = realloc(lexer, lexer->cap);
    }

    lexer->tokens[lexer->len++] = (Token){
        type,
        value
    };
}
