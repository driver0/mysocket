#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>		/* basic socket defns */
#include <netinet/in.h>		/* sockaddr_in{} and other Internet defns, such as htons() */
#include <arpa/inet.h>		/* inet funcs, such as inet_pton, inet_ntop */
#include "cli_serv.h"

int main(int argc, char **argv)
{
	int sockfd;
	struct  sockaddr_in servaddr;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <IPaddress>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	/* create an active socket */
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	/* connect the server */
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
		perror("connect");
		exit(EXIT_FAILURE);
	}
	str_cli(stdin, sockfd);		/* do it all */

	return 0;
}
