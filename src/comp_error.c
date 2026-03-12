#include "comp_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


static Program_Data program;
static int line_len = 5;


void set_program_data(const char *data, const char *name) {
    program = (Program_Data){
        data,
        name
    };
}


void comp_error(const char *compenent, const char *error, int line, int col) {
    if (line == -1 || col == -1) {
        printf("g-- %s: %s\n", compenent, error);
    } else {
        size_t i = 0; 
        int num;
        if (line == 0) {
            size_t last = 0;
            ++line;
            while (program.data[i] != '\0') {
                if (program.data[i++] == '\n' && program.data[i] != '\0') {
                    last = i;
                    ++line;
                }
            }
            i = last;
        } else {
            num = line - 1;
            while ((num -= program.data[i++] == '\n'));
        }

        if (col == 0) {
            while (program.data[i + col] != '\n' && program.data[i + col] != '\0')
                ++col;
            ++col;
        }

        printf("g-- %s %s:%d:%d: %s\n", compenent, program.name, line, col, error);
        

        if (line_len < floor(log10(line)))
            line_len = floor(log10(line));
        sscanf(program.data + i, "%*[^\n]%n\n", &num);
        printf("%*d | %.*s\n", line_len, line, num, program.data + i);
        printf("%*s | %*s^\n", line_len, "", col - 1, "");
    }

    puts("Compilation halting...");

    exit(-1);
}
