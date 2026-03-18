#include "emitter.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


Emitter *emit(Node global_scope) {
    Emitter *emitter = emitter_init();
    emitter_precat(emitter, "#include <stdlib.h>\n#include <stdio.h>\n\n");

    Node val = global_scope.left[0];
    emitter_precat(emitter, "char *data;\n");
    emitter_cat(emitter, "int main(void) {\ndata = calloc(sizeof(char), ");
    char temp[64];
    sprintf(temp, "%zu", val.left->value);
    emitter_cat(emitter, temp);
    emitter_cat(emitter, ");\n");

    emit_scope(global_scope.left, global_scope.value, emitter);

    emitter_cat(emitter, "free(data);\nreturn 0;\n}\n\n");
    
    for (size_t i = 0; i < emitter->len; ++i) {
        sprintf(temp, "void function%zu(void) ", i);
        emitter_cat(emitter, temp);
        emit_statement(emitter->functions + i, emitter);
    }

    return emitter;
}


void emit_statement(Node *statement, Emitter *emitter) {
    switch (statement->type) {
        case ND_SCOPE:
            emitter_cat(emitter, "{\n");
            emit_scope(statement->left, statement->value, emitter);
            emitter_cat(emitter, "}");
            break;

        case ND_EXPRESSION:
            emit_expr(statement->left, emitter);
            break;

        case ND_OUTPUT:
            emitter_cat(emitter, "printf(\"%s\", data + ");
            emit_expr(statement->left, emitter);
            emitter_cat(emitter, ")");
            break;
        
        case ND_OUTPUT_INT:
            emitter_cat(emitter, "printf(\"%u\", ");
            emit_expr(statement->left, emitter);
            emitter_cat(emitter, ")");
            break;

        case ND_IF:
            emitter_cat(emitter, "if (");
            emit_expr(statement->comp, emitter);
            emitter_cat(emitter, ")");
            if (statement->left != NULL)
                emit_statement(statement->left, emitter);
            break;
        
        case ND_WHILE:
            emitter_cat(emitter, "while (");
            emit_expr(statement->comp, emitter);
            emitter_cat(emitter, ")");
            if (statement->left != NULL)
                emit_statement(statement->left, emitter);
            break;

        default:
            break;
    }
    emitter_cat(emitter, ";\n");
    
}


void emit_scope(Node *statements, size_t len, Emitter *emitter) {
    size_t ptr = 0;

    while (ptr < len) {
        emit_statement(statements + ptr++, emitter);
    }
    
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

        case ND_EXECUTE:
            emitter_cat(emitter, "((void (*)(void))*(size_t *)&(");
            emit_expr(statement->left, emitter);
            emitter_cat(emitter, "))()");
            break;

        case ND_NAND:
            emitter_cat(emitter, "~((");
            emit_expr(statement->left, emitter);
            emitter_cat(emitter, ") & (");
            emit_expr(statement->right, emitter);
            emitter_cat(emitter, "))");
            break;

        case ND_ASSIGNMENT:
            if (statement->right->type == ND_SCOPE) {
                sprintf(temp, "function%zu", emitter->len);

                emitter_precat(emitter, "void ");
                emitter_precat(emitter, temp);
                emitter_precat(emitter, "(void);\n");

                function_push(emitter, statement->right);

                emit_statement(statement->right, emitter);
                emitter_cat(emitter, "*(size_t *)&(");
                emit_expr(statement->left, emitter);
                emitter_cat(emitter, ") = (size_t)&");
                emitter_cat(emitter, temp);
                
                statement->right = NULL;
            } else {
                emitter_cat(emitter, "(");
                emit_expr(statement->left, emitter);
                emitter_cat(emitter, ") = (");
                emit_expr(statement->right, emitter);
                emitter_cat(emitter, ")");
            }
            break;

        default:
            break;
    }
}


Emitter *emitter_init(void) {
    Emitter *emitter = malloc(sizeof(Emitter));
    emitter->ptr = 0;

    emitter->preamble.len = 0;
    emitter->preamble.cap = 256;
    emitter->preamble.string = calloc(sizeof(char), emitter->preamble.cap);

    emitter->main_body.len = 0;
    emitter->main_body.cap = 256;
    emitter->main_body.string = calloc(sizeof(char), emitter->main_body.cap);

    emitter->len = 0;
    emitter->cap = 4;
    emitter->functions = malloc(sizeof(Node) * emitter->cap);

    return emitter;
}


void emitter_free(Emitter *emitter) {
    free(emitter->preamble.string);
    free(emitter->main_body.string);
    for (size_t i = 0; i < emitter->len; ++i) {
        node_free(emitter->functions + i);
    }
    free(emitter->functions);
    free(emitter);
}


void emitter_precat(Emitter *emitter, const char string[]) {
    emitter->preamble.len += strlen(string);

    while (emitter->preamble.len > emitter->preamble.cap - 1) {
        emitter->preamble.cap <<= 1;
        emitter->preamble.string =  realloc(emitter->preamble.string, sizeof(char) * emitter->preamble.cap);
    }
    
    strcat(emitter->preamble.string, string);
}


void emitter_cat(Emitter *emitter, const char string[]) {
    emitter->main_body.len += strlen(string);

    while (emitter->main_body.len > emitter->main_body.cap - 1) {
        emitter->main_body.cap <<= 1;
        emitter->main_body.string =  realloc(emitter->main_body.string, sizeof(char) * emitter->main_body.cap);
    }
    
    strcat(emitter->main_body.string, string);
}


void function_push(Emitter *emitter, Node *function) {
    if (emitter->len >= emitter->cap) {
        emitter->cap <<= 1;
        emitter->functions = realloc(emitter->functions, sizeof(Node) * emitter->cap);
    }

    emitter->functions[emitter->len++] = *function;
    free(function);
}
