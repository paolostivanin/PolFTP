all: ftpclient ftpclient_gui

ftpclient: bin/ftpclient.c
	@echo "--> Compiling core..."
	@gcc -Wall -g -O1 -o ftpclient bin/ftpclient.c
	@echo "--> ...done"
	
ftpclient_gui: bin/gtk3/entry.c
	@echo "--> Compiling gui..."
	@gcc -Wall -g -O1 -o ftpclient_gui bin/gtk3/entry.c  `pkg-config --cflags --libs gtk+-3.0`
	@echo "--> ...done!"
