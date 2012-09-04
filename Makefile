CC = gcc
CFLAGS = -Wall -Wextra -Wformat-security -O2 -D_FORTIFY_SOURCE=2 -fstack-protector

CLANG_OR_GCC := $(shell which clang)
ifeq "$(CLANG_OR_GCC)" "/usr/bin/clang"
 VER := $(shell clang --version | clang --version | grep -o 3.1 | awk '0; NR == 1 { print $0 } ')
 ifeq "$(VER)" "3.1"
  CC = clang
 endif
endif

all: ftpclient ftpserver generate_sha256

ftpclient: bin/client/ftpclient.c bin/onexit.c bin/client/do_syst_cmd.c bin/client/do_cwd_cmd.c bin/client/do_pwd_cmd.c bin/client/do_dele_cmd.c bin/client/do_mkd_cmd.c bin/client/do_rmd_cmd.c bin/client/do_rnm_cmd.c bin/client/do_retr_cmd.c bin/client/do_stor_cmd.c bin/client/do_list_cmd.c
	@echo "--> Using ${CC} to compile... <--"
	@echo "--> Compiling client..."
	@$(CC) $(CFLAGS) -lcurl -o ftpclient bin/client/ftpclient.c bin/onexit.c bin/client/do_syst_cmd.c bin/client/do_cwd_cmd.c bin/client/do_pwd_cmd.c bin/client/do_dele_cmd.c bin/client/do_mkd_cmd.c bin/client/do_rmd_cmd.c bin/client/do_rnm_cmd.c bin/client/do_retr_cmd.c bin/client/do_stor_cmd.c bin/client/do_list_cmd.c
	@echo "--> ...done"
	
ftpserver: bin/server/ftpserver.c bin/onexit.c bin/server/do_server_syst_cmd.c bin/server/do_server_cwd_cmd.c bin/server/do_server_pwd_cmd.c bin/server/do_server_dele_cmd.c bin/server/do_server_mkd_cmd.c bin/server/do_server_rmd_cmd.c bin/server/do_server_rnm_cmd.c bin/server/do_server_retr_cmd.c bin/server/do_server_stor_cmd.c bin/server/do_server_list_cmd.c bin/server/list-files.c bin/server/free-file-list.c bin/server/get_syst.c bin/server/get_server_public_ip.c bin/server/check_login_details.c
	@echo "--> Compiling server..."
	@$(CC) $(CFLAGS) -lcurl -lcrypto -o ftpserver bin/server/ftpserver.c bin/onexit.c bin/server/do_server_syst_cmd.c bin/server/do_server_cwd_cmd.c bin/server/do_server_pwd_cmd.c bin/server/do_server_dele_cmd.c bin/server/do_server_mkd_cmd.c bin/server/do_server_rmd_cmd.c bin/server/do_server_rnm_cmd.c bin/server/do_server_retr_cmd.c bin/server/do_server_stor_cmd.c bin/server/do_server_list_cmd.c bin/server/list-files.c bin/server/free-file-list.c bin/server/get_syst.c bin/server/get_server_public_ip.c bin/server/check_login_details.c
	@echo "--> ...done"

generate_sha256: bin/generate_sha256.c
	@echo "-> Compiling generate_sha256..."
	@$(CC) $(CFLAGS) -lcrypto -o generate_sha256 bin/generate_sha256.c
	@echo "--> ...done"

ftpclient_gui: bin/gtk3/entry.c
	@echo "--> Compiling client GUI..."
	@gcc -Wall -Wextra -O2 -D_FORTIFY_SOURCE=2 -fstack-protector -o ftpclient_gui bin/gtk3/client_ui.c  `pkg-config --cflags --libs gtk+-3.0`
	@echo "--> ...done"
