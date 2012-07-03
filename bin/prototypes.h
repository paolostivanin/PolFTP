#ifndef PROTOTYPES_H_INCLUDED
#define PROTOTYPES_H_INCLUDED
void check_before_start(int, char **);
extern size_t file_list(char *, char ***);
extern void onexit(int, int, int, int);
void sig_handler(int, int, int, int);
#endif
