#ifndef PROTOTYPES_H_INCLUDED
#define PROTOTYPES_H_INCLUDED
void check_before_start(int argc, char *argv[]);
void onexit(int c, int s, int file, int flag);
void sig_handler(int signo, int sockd, int newsockd, int fd);
#endif
