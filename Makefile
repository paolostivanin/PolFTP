all: ftpclient ftpserver

ftpclient: bin/ftpclient.c bin/ftpclient.c bin/clear_buf.c
	@echo "--> Compiling client..."
	@gcc -Wall -Wextra -Wformat-security -O2 -D_FORTIFY_SOURCE=2 -fstack-protector -o ftpclient bin/ftpclient.c bin/onexit.c
	@echo "--> ...done"
	
ftpserver: bin/ftpserver.c bin/list-files.c bin/onexit.c bin/clear_buf.c
	@echo "--> Compiling server..."
	@gcc -Wall -Wextra -Wformat-security -O2 -D_FORTIFY_SOURCE=2 -fstack-protector -o ftpserver bin/ftpserver.c bin/list-files.c bin/get_syst.c bin/onexit.c
	@echo "--> ...done"

ftpclient_gui: bin/gtk3/entry.c
	@echo "--> Compiling client GUI..."
	@gcc -Wall -Wextra -O2 -D_FORTIFY_SOURCE=2 -fstack-protector -o ftpclient_gui bin/gtk3/client_ui.c  `pkg-config --cflags --libs gtk+-3.0`
	@echo "--> ...done!"
