bin_PROGRAMS = ftpserver
ftpserver_SOURCES = ftpserver-fork.c ../onexit.c do_server_fork_syst_cmd.c do_server_fork_cwd_cmd.c do_server_fork_pwd_cmd.c do_server_fork_dele_cmd.c do_server_fork_mkd_cmd.c do_server_fork_rmd_cmd.c do_server_fork_rnm_cmd.c do_server_fork_retr_cmd.c do_server_fork_stor_cmd.c do_server_fork_list_cmd.c list-files.c free-file-list.c get_syst.c get_server_public_ip.c check_login_details.c server_errors_handler.c
ftpserver_CFLAGS = $(AM_CFLAGS)
ftpserver_LDADD = -lcurl -lcrypto
