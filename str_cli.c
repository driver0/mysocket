#include "rdwr/rdwr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/select.h>
#include <sys/time.h>

void str_cli(FILE *fp, int sockfd)
{
	int maxfdp1, res;
	fd_set rset;
	ssize_t n;
	char sendline[LINE_MAX], recvline[LINE_MAX];

	FD_ZERO(&rset);

	while (1) {
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
			if ( (n = readline(sockfd, recvline, sizeof(recvline))) == -1) {
				perror("readline");
				exit(EXIT_FAILURE);
			} else if (n == 0) {
				fprintf(stderr, "str_cli: server terminated prematurely\n");
				exit(EXIT_FAILURE);
			}
			fputs(recvline, stdout);
		}
		if (FD_ISSET(fileno(fp), &rset)) {	/* input is readable */
			if (fgets(sendline, sizeof(sendline), fp) == NULL)
				return ;	/* all done */
			if (writen(sockfd, sendline, strlen(sendline)) == -1) {
				perror("writen");
				exit(EXIT_FAILURE);
			}
		}
	}
}
