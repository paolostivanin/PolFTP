#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]){
	char *dir = argv[1];
    	chdir(dir);
    	system("ls");
    	return 0;
}
