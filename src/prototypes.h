#ifndef PROTOTYPES_H_INCLUDED
#define PROTOTYPES_H_INCLUDED
#include <stdint.h>
#define VERSION "1.1.0-alpha1"
//
//Client's functions
int do_syst_cmd(const int);
int do_pwd_cmd(const int);
int do_cwd_cmd(const int);
int do_list_cmd(const int);
int do_retr_cmd(const int);
int do_stor_cmd(const int);
int do_dele_cmd(const int);
int do_mkd_cmd(const int);
int do_rmd_cmd(const int);
int do_rnm_cmd(const int);
void client_errors_handler(const int, const int);
//
//Concurrent server's functions

int get_syst(char **);
int file_list(char *, char ***);
int do_server_fork_syst_cmd(const int);
int do_server_fork_pwd_cmd(const int);
int do_server_fork_cwd_cmd(const int);
int do_server_fork_list_cmd(const int); /* chiamare free_file_list(&POINTER_USED, $NUM_OF_ELEMENTS) al termine */
int do_server_fork_retr_cmd(const int);
int do_server_fork_stor_cmd(const int);
int do_server_fork_dele_cmd(const int);
int do_server_fork_mkd_cmd(const int);
int do_server_fork_rmd_cmd(const int);
int do_server_fork_rnm_cmd(const int);
int check_login_details(char *, char *);
char *get_public_ip(void); /* liberare la memoria al termine con free($POINTER_USED) */
void do_child(const int);
void free_file_list(char ***, uint32_t);
void sig_handler(const int, const int, const int);
void server_errors_handler(const int, const int);
//
//Common functions
void check_before_start(int, char **);
void onexit(int, int, int, int);
//
/*Iterative server's functions
void sig_handler(const int, const int, const int);
void get_syst(char **);
uint32_t file_list(char *, char ***);
void free_file_list(char ***, uint32_t);
void do_server_syst_cmd(const int, const int);
void do_server_pwd_cmd(const int, const int);
void do_server_cwd_cmd(const int, const int);
void do_server_list_cmd(const int, const int);  chiamare free_file_list(&POINTER_USED, $NUM_OF_ELEMENTS) al termine
void do_server_retr_cmd(const int, const int);
void do_server_stor_cmd(const int, const int);
void do_server_dele_cmd(const int, const int);
void do_server_mkd_cmd(const int, const int);
void do_server_rmd_cmd(const int, const int);
void do_server_rnm_cmd(const int, const int);
char *get_public_ip(void);  liberare la memoria al termine con free($POINTER_USED)
int check_login_details(char *, char *);
*/
#endif
