all: ftpclient ftpclient_gui

ftpclient: bin/ftpclient.c
	@echo "--> Compiling core..."
	@gcc bin/ftpclient.c -o ftpclient
	@echo "--> ...done"
	
ftpclient_gui: bin/gtk3/entry.c
	@echo "--> Compiling gui..."
	@gcc bin/gtk3/entry.c -o ftpclient_gui `pkg-config --cflags --libs gtk+-3.0`
	@echo "--> ...done!"
