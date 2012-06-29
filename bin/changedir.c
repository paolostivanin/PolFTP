#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]){
	char *dir = argv[1];
    	if(chdir(dir) == 0){
    		if(system("ls") == -1){
    			perror("system");
    			return 1;
    		}
    	}
    	else{
    		perror("chdir");
    		return 1;
    	}
    	return 0;
}
