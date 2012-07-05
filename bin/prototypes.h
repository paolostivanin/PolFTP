#ifndef PROTOTYPES_H_INCLUDED
#define PROTOTYPES_H_INCLUDED
#include <stdint.h>
void check_before_start(int, char **);
void sig_handler(int, int, int, int);
extern void clear_buf(char *, char *, char *, int);
extern uint32_t file_list(char *, char ***);
extern void onexit(int, int, int, int);
#endif
