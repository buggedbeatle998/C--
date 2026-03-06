#include "comp_error.h"


static Program_Data program;


void set_program_data(const char *data, const char *name) {
    program = (Program_Data){
        data,
        name
    };
}


void comp_error(const char *compenent, const char *error, int line, int col) {
    printf("g-- %s %s:%d:%d: %s\n", compenent, program.name, line, col, error);
    
    size_t i = 0;
    int num = line - 1;
    while ((num -= program.data[i++] == '\n'));

    sscanf(program.data + i, "%*[^\n]\n%n", &num);
    printf("%.*s", num, program.data + i);
    printf("%*s^\n", col - 1, "");
    puts("Compilation halting...\n");

    exit(-1);
}
