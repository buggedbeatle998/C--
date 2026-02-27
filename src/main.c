#include <stdio.h>
#include "lexer.h"


int main(void) {
    FILE *f = fopen("../test.cmm", "r");
    if (f) {
        fseek(f, 0, SEEK_END);
        size_t len = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *buff = malloc(sizeof(char) * (len + 1));
        fread(buff, len, 1, f);
        fclose(f);
        Lexer *lexer = lex((const char *)buff);
        for (size_t i = 0; i < lexer->len; ++i) {
            printf("%d %llu\n", lexer->tokens[i].type, lexer->tokens[i].value);
        }
    } else {
        printf("No file found!\n");
    }

    return 0;
}
