#ifndef H_EMITTER
#define H_EMITTER

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "parser.h"


typedef struct {
    size_t len;
    size_t cap;
    char *string;
} Program;


typedef struct {
    size_t ptr;
    Program program;
} Emitter;


Emitter *emit(Node *statements, size_t len);
void emit_expr(Node *statement, Emitter *emitter);

Emitter *emitter_init(void);
void emitter_free(Emitter *emitter);
void emitter_cat(Emitter *emitter, const char *string);

#endif
