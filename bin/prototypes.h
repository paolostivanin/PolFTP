#ifndef PROTOTYPES_H_INCLUDED
#define PROTOTYPES_H_INCLUDED
#include <stdint.h>
void check_before_start(int, char **);
void sig_handler(int, int, int, int);
void get_syst(char **);
uint32_t file_list(char *, char ***);
void free_file_list(char ***, uint32_t);
void do_syst_cmd(int);
void do_pwd_cmd(int);
void do_cwd_cmd(int);
void do_list_cmd(int);
void do_retr_cmd(int);
void do_stor_cmd(int);
void do_dele_cmd(int);
void do_mkd_cmd(int);
void do_rmd_cmd(int);
void do_rnm_cmd(int);
void onexit(int, int, int, int);
#endif
