#ifndef _CLI_SERV_H
#define _CLI_SERV_H

#include <stdio.h>

#define SERV_PORT 	9877

typedef void Sigfunc(int);

extern void str_echo(int sockfd);
extern void str_cli(FILE *fp, int sockfd);
extern Sigfunc *signal(int signo, Sigfunc *func);
extern void sig_chld(int signo);

#endif /* _CLI_SERV_H */
