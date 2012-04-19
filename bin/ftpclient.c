#include <stdio.h>
#include <stdlib.h>

#define LEN 25

int main(){
char host[LEN],user[LEN],pwd[LEN];
puts("Inserisci <host user pwd>:");
scanf("%s %s %s", host,user,pwd);
printf("Host: %s\nUser: %s\nPWD: %s\n", host,user,pwd);
return EXIT_SUCCESS;
}
