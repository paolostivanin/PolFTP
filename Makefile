all: ftpclient ftpserver

ftpclient: ftpclient.c ftpclient.c clear_buf.c
	@echo "--> Compiling client..."
	@gcc -Wall -Wextra -g -O2 -D_FORTIFY_SOURCE=2 -fstack-protector -o ftpclient ftpclient.c onexit.c
	@echo "--> ...done"
	
ftpserver: ftpserver.c list-files.c onexit.c clear_buf.c
	@echo "--> Compiling server..."
	@gcc -Wall -Wextra -O2 -D_FORTIFY_SOURCE=2 -fstack-protector -o ftpserver ftpserver.c list-files.c get_syst.c onexit.c
	@echo "--> ...done"
