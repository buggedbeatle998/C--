#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "emitter.h"


int main(void) {
    FILE *f = fopen("../test.cmm", "r");
    if (f) {
        fseek(f, 0, SEEK_END);
        size_t len = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *program = malloc(sizeof(char) * (len + 1));
        fread(program, sizeof(char), len, f);
        fclose(f);
        
        Lexer *lexer = lex(program, len);
        printf("Lexed!\n");
        //for (size_t i = 0; i < lexer->len; ++i) {
        //    printf("%d %llu\n", lexer->tokens[i].type, lexer->tokens[i].value);
        //}
        //printf("\n");
        
        Parser *parser = parse(lexer->tokens, lexer->len);
        lexer_free(lexer);
        printf("Parsed!\n");
        //for (size_t i = 0; i < parser->ast.len; ++i) {
        //    printf("%d\n", parser->ast.statements[i].type);
        //    //printf("%d\n", parser->ast.statements[i].left->type);
        //    //printf("%d\n", parser->ast.statements[i].right->type);
        //}

        Emitter *emitter = emit(parser->ast.statements, parser->ast.len);
        parser_free(parser);
        printf("Emitted!\n");
        
        f = fopen("../test.c", "w");
        if (f) {
            fprintf(f, emitter->program.string);
            fclose(f);
            printf("Written!\n");
        }

        emitter_free(emitter);

    } else {
        printf("No file found!\n");
    }

    return 0;
}
