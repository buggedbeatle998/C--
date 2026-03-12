#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "comp_error.h"
#include "lexer.h"
#include "parser.h"
#include "emitter.h"


int main(int argc, char *const argv[]) {
    char c;
    char *output = "a.out";
    bool renamed = false;
    bool use_c = false;
    bool verbose = false;
    bool debug = false;
    while ((c = getopt(argc, argv, "h?o:cvd")) != -1) {
        switch (c) {
            case '?':
            case 'h':
                printf(
                   "Usage: g-- [options] file...\n" \
                   "Options:\n" \
                   "  -? -h\r\t\tDisplay this information.\n" \
                   "  -o <file>\r\t\tPlace the output into <file>.\n" \
                   "  -c\r\t\tCompile to C.\n" \
                   "  -v\r\t\tDisplay information as the compiler runs.\n" \
                   "  -d\r\t\tTurn on all debug options.\n"
                );
                return 0;

            case 'o':
                output = optarg;
                renamed = true;
                break;
            
            case 'c':
                if (!renamed)
                    output = "a.c";
                use_c = true;
                break;

            case 'v':
                verbose = true;
                break;

            case 'd':
                debug = true;
                break;

            default:
                printf("Invalid flag %c\n", c);
                return -1;
        }
    }

    FILE *f = fopen(argv[optind], "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        size_t len = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *buff = malloc(sizeof(char) * (len + 1));
        fread(buff, sizeof(char), len, f);
        buff[len] = '\0';
        fclose(f);

        set_program_data(buff, argv[optind]);
        
        if (verbose)
            printf("Lexing...\n");
        Lexer *lexer = lex(buff, len);
        if (verbose)
            printf("Lexed!\n");

        if (debug) {
            printf("%zu\n", lexer->len);
            for (size_t i = 0; i < lexer->len; ++i) {
                printf("%d %zu\n", lexer->tokens[i].type, lexer->tokens[i].value);
            }
            printf("\n");
        }
        
        if (verbose)
            printf("Parsing...\n");
        Parser *parser = parse(lexer);
        lexer_free(lexer);
        if (verbose)
            printf("Parsed!\n");

        if (debug) {
            printf("%zu\n", parser->global_scope.value);
            for (size_t i = 0; i < parser->global_scope.value; ++i) {
                printf("%d", parser->global_scope.left[i].type);
                if (parser->global_scope.left[i].left != NULL)
                    printf(" %d", parser->global_scope.left[i].left->type);
                if (parser->global_scope.left[i].right != NULL)
                    printf(" %d", parser->global_scope.left[i].right->type);
                printf("\n");
            }
            printf("\n");
        }

        if (verbose)
            printf("Emitting...\n");
        Emitter *emitter = emit(parser->global_scope);
        parser_free(parser);
        if (verbose)
            printf("Emitted!\n");
        
        free(buff);
        
        if (use_c) {
            f = fopen(output, "w");
            if (f) {
                fputs(emitter->program.string, f);
                fclose(f);
                if (verbose)
                    printf("File \"%s\" successfully compiled to C file \"%s\"!\n", argv[optind], output);
            }
        } else {
// TODO Add MSVC
// TODO Add functions
            if (debug)
#if defined(__GNUC__)
                puts("gcc");
            buff = malloc(strlen("gcc -x c -o  -") + strlen(output) + 1);
            sprintf(buff, "gcc -x c -o %s -", output);
#elif defined(__clang__)
                puts("clang");
            buff = malloc(strlen("clang -x c -w -o  -") + strlen(output) + 1);
            sprintf(buff, "clang -x c -w -o %s -", output);
#else
                ;
            emitter_free(emitter);
            puts(
                "Could not find a suitable C compiler!\n" \
                "Halting compilation...\n"
            );
            return -1;
#endif
            f = popen(buff, "w");
            fwrite(emitter->program.string, sizeof(char), emitter->program.len, f);
            pclose(f);
            free(buff);
            if (debug) {
                f = fopen("g--test.c", "w");
                fwrite(emitter->program.string, sizeof(char), emitter->program.len, f);
                fclose(f);
            }
            if (verbose)
                printf("File \"%s\" successfully compiled to executable \"%s\"!\n", argv[optind], output);
        }

        emitter_free(emitter);
    } else {
        printf("No file \"%s\" found!\n", argv[optind]);
    }

    return 0;
}
