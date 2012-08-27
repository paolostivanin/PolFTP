#ifndef PROTOTYPES_H_INCLUDED
#define PROTOTYPES_H_INCLUDED
#include <stdint.h>
//
//Client's functions
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
//
//Server's functions
void sig_handler(int, int, int, int);
void get_syst(char **);
uint32_t file_list(char *, char ***);
void free_file_list(char ***, uint32_t);
void do_server_syst_cmd(int, int);
void do_server_pwd_cmd(int, int);
void do_server_cwd_cmd(int, int);
void do_server_list_cmd(int, int);
void do_server_retr_cmd(int, int);
void do_server_stor_cmd(int, int);
void do_server_dele_cmd(int, int);
void do_server_mkd_cmd(int, int);
void do_server_rmd_cmd(int, int);
void do_server_rnm_cmd(int, int);
//
//Common functions
void check_before_start(int, char **);
void onexit(int, int, int, int);
#endif