#include "rdwr/rdwr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

void str_cli(FILE *fp, int sockfd)
{
	int maxfdp1, res, stdineof = 0;
	fd_set rset;
	ssize_t n;
	char buf[LINE_MAX];

	FD_ZERO(&rset);

	while (1) {
		if (stdineof == 0)
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		if (fileno(fp) < sockfd)
			maxfdp1 = sockfd + 1;
		else
			maxfdp1 = fileno(fp) + 1;

		if ( (res = select(maxfdp1, &rset, NULL, NULL, NULL)) == -1) {
			perror("select");
			exit(EXIT_FAILURE);
		}
		if (FD_ISSET(sockfd, &rset)) {	/* socket is readable */
			if ( (n = read(sockfd, buf, sizeof(buf))) == -1) {
				perror("read");
				exit(EXIT_FAILURE);
			} else if (n == 0) {
				if (stdineof == 1)
					return ;		/* normal termination */
				else {
					fprintf(stderr, "str_cli: server terminate prematurely\n");
					exit(EXIT_FAILURE);
				}
			}
			if (write(fileno(stdout), buf, n) == -1) {
				perror("write");
				exit(EXIT_FAILURE);
			}
		}
		if (FD_ISSET(fileno(fp), &rset)) {	/* input is readable */
			if ( (n = read(fileno(fp), buf, sizeof(buf))) == -1) {
				perror("read");
				exit(EXIT_FAILURE);
			} else if (n == 0) {
				stdineof = 1;
				shutdown(sockfd, SHUT_WR);
				FD_CLR(fileno(fp), &rset);
				continue;
			}
			if ( write(fileno(stdout), buf, n) == -1) {
				perror("write");
				exit(EXIT_FAILURE);
			}
		}
	}
}
