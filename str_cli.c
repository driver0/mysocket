#include "rdwr/rdwr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

void str_cli(FILE *fp, int sockfd)
{
	ssize_t n;
	char sendline[LINE_MAX], recvline[LINE_MAX];

	while ( fgets(sendline, sizeof(sendline), fp) != NULL) {
		if (writen(sockfd, sendline, strlen(sendline)) == -1) {
			perror("writen");
			exit(EXIT_FAILURE);
		}
		if ( (n = readline(sockfd, recvline, sizeof(recvline)) ) == -1) {
			perror("readline");
			exit(EXIT_FAILURE);
		} else if (n == 0) {
			fprintf(stdout, "str_cli: server terminated permaturely\n");
			exit(EXIT_FAILURE);
		}
		fputs(recvline, stdout);
	}
}
