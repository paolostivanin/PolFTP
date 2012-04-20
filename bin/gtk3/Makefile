all: ftp_input_info_non_resizable ftp_input_info_resizable

ftp_input_info_non_resizable: entry.c
	@echo "--> Compiling first program..."
	@gcc entry.c -o ftp_input_info_non_resizable `pkg-config --cflags --libs gtk+-3.0`
	@echo "--> ...done..."
	
ftp_input_info_resizable: entry_table.c
	@echo "--> Compiling second program..."
	@gcc entry_table.c -o ftp_input_info_resizable `pkg-config --cflags --libs gtk+-3.0`
	@echo "--> ...all done!"
