#include "rdwr/rdwr.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

void str_echo(int sockfd)
{
	ssize_t n;
	char buf[LINE_MAX];

	while (1) {
		if ( (n = readline(sockfd, buf, sizeof(buf))) == -1) {
			perror("readline");
			exit(EXIT_FAILURE);
		} else if (n == 0) {
			return ;		/* connection closed by other end */
		}
		if (writen(sockfd, buf, n) == -1) {
			perror("writen");
			exit(EXIT_FAILURE);
		}
	}
}
