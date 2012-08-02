#ifndef PROTOTYPES_H_INCLUDED
#define PROTOTYPES_H_INCLUDED
#include <stdint.h>
void check_before_start(int, char **);
void sig_handler(int, int, int, int);
void get_syst(char **);
uint32_t file_list(char *, char ***);
void onexit(int, int, int, int);
#endif
