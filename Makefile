all: ftpclient ftpserver ftpclient_gui

ftpclient: bin/ftpclient.c
	@echo "--> Compiling client..."
	@gcc -Wall -O3 -D_FORTIFY_SOURCE=2 -o ftpclient bin/ftpclient.c bin/clear_buf.c
	@echo "--> ...done"
	
ftpclient: bin/ftpserver.c
	@echo "--> Compiling server..."
	@gcc -Wall -O3 -D_FORTIFY_SOURCE=2 -o ftpserver bin/ftpserver.c bin/list-files.c bin/onexit.c bin/clear_buf.c
	@echo "--> ...done"
	
ftpclient_gui: bin/gtk3/entry.c
	@echo "--> Compiling client GUI..."
	@gcc -Wall -g -O2 -D_FORTIFY_SOURCE=2 -o ftpclient_gui bin/gtk3/client_ui.c  `pkg-config --cflags --libs gtk+-3.0`
	@echo "--> ...done!"
