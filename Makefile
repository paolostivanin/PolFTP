CC = gcc
CFLAGS = -Wall -Wextra -Wformat-security -O2 -D_FORTIFY_SOURCE=2 -fstack-protector

CLANG_OR_GCC := $(shell which clang)
ifeq "$(CLANG_OR_GCC)" "/usr/bin/clang"
 VER := $(shell clang --version | clang --version | grep -o 3.1 | awk '0; NR == 1 { print $0 } ')
 ifeq "$(VER)" "3.1"
  CC = clang
 endif
endif

all: ftpclient ftpserver

ftpclient: bin/ftpclient.c bin/ftpclient.c bin/onexit.c
	@echo "Using ${CC}..."
	@echo "--> Compiling client..."
	@$(CC) $(CFLAGS) -o ftpclient bin/ftpclient.c bin/onexit.c
	@echo "--> ...done"
	
ftpserver: bin/ftpserver.c bin/list-files.c bin/onexit.c bin/get_syst.c
	@echo "--> Compiling server..."
	@$(CC) $(CFLAGS) -o ftpserver bin/ftpserver.c bin/list-files.c bin/get_syst.c bin/onexit.c
	@echo "--> ...done"

ftpclient_gui: bin/gtk3/entry.c
	@echo "--> Compiling client GUI..."
	@gcc -Wall -Wextra -O2 -D_FORTIFY_SOURCE=2 -fstack-protector -o ftpclient_gui bin/gtk3/client_ui.c  `pkg-config --cflags --libs gtk+-3.0`
	@echo "--> ...done!"
