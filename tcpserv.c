#include <stdio.h>
#include <stdlib.h>
#include <string.h>			/* mem_* funcs */
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>		/* basic socket defns */
#include <netinet/in.h>		/* sockaddr_in{} and other Internet defns, such as htons() */
#include <arpa/inet.h>		/* inet funcs, such as inet_pton, inet_ntop */
#include <limits.h>
#include <signal.h>
#include "cli_serv.h"

#define LISTENQ 		1024	/* 2nd argument to listen() */

int main(int argc, char **argv)
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	char buf[LINE_MAX];
	struct sockaddr_in cliaddr, servaddr;

	/* create an active socket */
	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	/* assign local protocol adddress to socket */
	if ( bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	/* translate unconnected active socket to passive socket */
	if ( listen(listenfd, LISTENQ) == -1) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	
	/* register handler function for signal SIGCHLD */
	if (signal(SIGCHLD, sig_chld) == SIG_ERR) {		/* must call waitpid with WNOHANG option. */
		perror("signal");
		exit(EXIT_FAILURE);
	}
	while (1) {
		clilen = sizeof(cliaddr);
		/* get the head of connected queue created by listen() for socket */
		if ( (connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) == -1) {
			if (errno == EINTR) {
				continue;
			} else {
				perror("accept");
				exit(EXIT_FAILURE);
			}
		}
		printf("Welcom %s: %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)), ntohs(cliaddr.sin_port));

		if ( (childpid = fork()) == 0) {	/* child process deals with the new connection */
			close(listenfd);		/* close listening socket descriptor */
			str_echo(connfd);		/* process the request */
			close(connfd);
			exit(EXIT_SUCCESS);
		}

		/* main process closes connected socket descriptor, so that it's using count will decrease */
		close(connfd);
	}
}
