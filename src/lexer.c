#include "lexer.h"

#include <stdint.h>
#include <stdbool.h>

#include "comp_error.h"


Lexer *lex(const char *program, size_t len) {
    Lexer *lexer = lexer_init();
    char val;


    while (lexer->ptr < len) {
        val = program[lexer->ptr];
        switch (val) {
            case '\n':
            case ' ':
            case '\t':
                // Whitespace is ignored
                break;

            case ';':
                lexer_push(lexer, TK_NEWLINE, 0);
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
                lex_num(program, len, lexer);
                break;

            case '=':
                lexer_push(lexer, TK_ASSIGNMENT, 0);
                break;

            case '^':
                lexer_push(lexer, TK_NAND, 0);
                break;

            case '>':
                if (program[lexer->ptr + 1] == '0') {
                    ++lexer->ptr;
                    lexer_push(lexer, TK_OUTPUT_INT, 0);
                } else {
                    lexer_push(lexer, TK_OUTPUT, 0);
                }
                break;

            case '(':
                lexer_push(lexer, TK_LBRACKET, 0);
                break;

            case ')':
                lexer_push(lexer, TK_RBRACKET, 0);
                break;

            case '[':
                lexer_push(lexer, TK_LSBRACKET, 0);
                break;

            case ']':
                lexer_push(lexer, TK_RSBRACKET, 0);
                break;

            case '{':
                lexer_push(lexer, TK_LCBRACKET, 0);
                break;

            case '}':
                lexer_push(lexer, TK_RCBRACKET, 0);
                break;

            default:
                if ('a' <= val && val <= 'z') {
                    lex_keyword(program, len, lexer);
                    break;
                }
                lexer_free(lexer);
                comp_error("Lexer", "Unrecognised Token!", lexer->line, lexer->col);
                
        }
        ++lexer->ptr;
        lexer_next(lexer);
        if (val == '\n')
            lexer_newline(lexer);
    }

    lexer_push(lexer, TK_EOF, 0);

    return lexer;
}


void lex_keyword(const char *program, size_t len, Lexer *lexer) {
    size_t str_len = 0;
    char *string = malloc(sizeof(char) * 64);
    char val;
    
    while (lexer->ptr < len) {
        val = program[lexer->ptr];
        if (val < 'a' || val > 'z')
            break;
        
        string[str_len++] = val;
        ++lexer->ptr;
    }
    string[str_len] = '\0';
    --lexer->ptr;

    lexer_push_str(lexer, TK_KEYWORD, string);
}


void lex_num(const char *program, size_t len, Lexer *lexer) {
    size_t total = 0;
    char val;
    
    while (lexer->ptr < len) {
        val = program[lexer->ptr];
        if (val < '0' || val > '9')
            break;
        
        total *= 10;
        total += val - '0';
        ++lexer->ptr;
    }
    --lexer->ptr;

    lexer_push(lexer, TK_VALUE, total);
}


Lexer *lexer_init(void) {
    Lexer *lexer = malloc(sizeof(Lexer));
    lexer->len = 0;
    lexer->cap = 64;
    lexer->tokens = malloc(sizeof(Token) * lexer->cap);
    lexer->ptr = 0;
    lexer->line_ptr = 0;
    lexer->line = 1;
    lexer->col = 1;

    return lexer;
}


void lexer_free(Lexer *lexer) {
    for (size_t i = 0; i < lexer->len; ++i) {
        if (lexer->tokens[i].type == TK_KEYWORD)
            free((char *)lexer->tokens[i].value);
    }
    free(lexer->tokens);
    free(lexer);
}


void lexer_next(Lexer *lexer) {
    lexer->col = lexer->ptr - lexer->line_ptr + 1;
}

void lexer_newline(Lexer *lexer) {
    ++lexer->line;
    lexer->line_ptr = lexer->ptr;
    lexer->col = 1;
}


void lexer_push(Lexer *lexer, TK_TYPE type, size_t value) {
    if (lexer->len >= lexer->cap) {
        lexer->cap <<= 1;
        lexer->tokens = realloc(lexer->tokens, sizeof(Token) * lexer->cap);
    }

    Token temp = (Token){
        type,
        lexer->line,
        lexer->col,
    };
    temp.value = value;
    lexer->tokens[lexer->len++] = temp;
}


void lexer_push_str(Lexer *lexer, TK_TYPE type, char *value) {
    if (lexer->len >= lexer->cap) {
        lexer->cap <<= 1;
        lexer->tokens = realloc(lexer->tokens, sizeof(Token) * lexer->cap);
    }

    Token temp = (Token){
        type,
        lexer->line,
        lexer->col,
    };
    temp.str_val = value;
    lexer->tokens[lexer->len++] = temp;
}
