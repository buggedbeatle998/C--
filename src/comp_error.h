#ifndef H_COMP_ERROR
#define H_COMP_ERROR


typedef struct {
    const char *data;
    const char *name;
} Program_Data;


void set_program_data(const char *data, const char *name);
void comp_error(const char *compenent, const char *error, int line, int col);

#endif
