all: ftpclient ftpserver ftpclient_gui

ftpclient: bin/ftpclient.c
	@echo "--> Compiling client..."
	@gcc -Wall -g -O2 -o ftpclient bin/ftpclient.c
	@echo "--> ...done"
	
ftpclient: bin/ftpserver.c
	@echo "--> Compiling server..."
	@gcc -Wall -g -O2 -o ftpserver bin/ftpserver.c
	@echo "--> ...done"
	
ftpclient_gui: bin/gtk3/entry.c
	@echo "--> Compiling client GUI..."
	@gcc -Wall -g -O2 -o ftpclient_gui bin/gtk3/client_ui.c  `pkg-config --cflags --libs gtk+-3.0`
	@echo "--> ...done!"
