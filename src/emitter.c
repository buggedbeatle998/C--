#include "emitter.h"


Emitter *emit(Node *statements, size_t len) {
    Emitter *emitter = emitter_init();
    emitter_cat(emitter, "#include <stdlib.h>\n#include <stdio.h>\n\nint main(void) {\n");

    Node val = statements[emitter->ptr];
    emitter_cat(emitter, "char *data = calloc(sizeof(char), ");
    char temp[64];
    sprintf(temp, "%zu", val.left->value);
    emitter_cat(emitter, temp);
    emitter_cat(emitter, ");\n");

    while (emitter->ptr < len) {
        val = statements[emitter->ptr++];
        switch (val.type) {
            case ND_EXPRESSION:
                emit_expr(val.left, emitter);
                break;

            case ND_OUTPUT:
                emitter_cat(emitter, "printf(\"%s\", data + ");
                emit_expr(val.left, emitter);
                emitter_cat(emitter, ")");
                break;
            
            case ND_OUTPUT_INT:
                emitter_cat(emitter, "printf(\"%u\", ");
                emit_expr(val.left, emitter);
                emitter_cat(emitter, ")");
                break;

            default:
                break;
        }
        emitter_cat(emitter, ";\n");
    }

    emitter_cat(emitter, "free(data);\nreturn 0;\n}\n");
    return emitter;
}


void emit_expr(Node *statement, Emitter *emitter) {
    char temp[64];
    switch (statement->type) {
        case ND_VALUE:
            sprintf(temp, "%zu", statement->value);
            emitter_cat(emitter, temp);
            break;

        case ND_ADDRESS:
            emitter_cat(emitter, "data[");
            emit_expr(statement->left, emitter);
            emitter_cat(emitter, "]");
            break;

        case ND_NAND:
            emitter_cat(emitter, "~((");
            emit_expr(statement->left, emitter);
            emitter_cat(emitter, ") & (");
            emit_expr(statement->right, emitter);
            emitter_cat(emitter, "))");
            break;

        case ND_ASSIGNMENT:
            emitter_cat(emitter, "(");
            emit_expr(statement->left, emitter);
            emitter_cat(emitter, ") = (");
            emit_expr(statement->right, emitter);
            emitter_cat(emitter, ")");
            break;

        default:
            break;
    }
}


Emitter *emitter_init(void) {
    Emitter *emitter = malloc(sizeof(Emitter));
    emitter->ptr = 0;
    emitter->program.len = 0;
    emitter->program.cap = 256;
    emitter->program.string = calloc(sizeof(char), emitter->program.cap);

    return emitter;
}


void emitter_free(Emitter *emitter) {
    free(emitter->program.string);
    free(emitter);
}


void emitter_cat(Emitter *emitter, const char string[]) {
    while ((emitter->program.len += strlen(string)) > emitter->program.cap - 1) {
        emitter->program.cap <<= 1;
        emitter->program.string =  realloc(emitter->program.string, sizeof(char) * emitter->program.cap);
    }
    
    strcat(emitter->program.string, string);
}
