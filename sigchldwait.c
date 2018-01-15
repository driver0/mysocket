#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "cli_serv.h"

void sig_chld(int signo)
{
	pid_t pid;
	int stat;

	if (signo == SIGCHLD) {
		while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0) {
			printf("child %d terminate\n", pid);
		}
	}
	return ;
}
