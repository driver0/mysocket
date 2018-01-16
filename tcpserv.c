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
#include <sys/select.h>
#include "cli_serv.h"

#define LISTENQ 		1024	/* 2nd argument to listen() */

int main(int argc, char **argv)
{
	int 			i, maxi, maxfd, listenfd, connfd, sockfd;
	int 			nready, client[FD_SETSIZE];
	ssize_t n;
	fd_set rset, allset;
	char buf[LINE_MAX], addr_str[INET_ADDRSTRLEN];
	socklen_t clilen;
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

	maxfd = listenfd;			/* initialize */
	maxi = -1;					/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;			/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	
	while (1) {
		rset = allset;			/* structure assignment */
		if ( (nready = select(maxfd+1, &rset, NULL, NULL, NULL)) == -1) {
			perror("select");
			exit(EXIT_FAILURE);
		}

		if (FD_ISSET(listenfd, &rset)) {	/* new client connection */
			clilen = sizeof(cliaddr);
			if ( (connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) == -1) {
				perror("accept");
				exit(EXIT_FAILURE);
			}
#ifdef 	NOTDEF
			printf("new client: %s, port %d\n",
					inet_ntop(AF_INET, &cliaddr.sin_addr, addr_str, sizeof(addr_str)),
					ntohs(cliaddr.sin_port));
#endif
			for (i = 0; i < FD_SETSIZE; i++) {
				if (client[i] == -1) {
					client[i] = connfd;	/* save descriptor */
					break;
				}
			}
			if (i == FD_SETSIZE) {
				fprintf(stderr, "too many clients\n");
				exit(EXIT_FAILURE);
			}
			FD_SET(connfd, &allset);	/* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd;			/* for select */
			if (i > maxi)
				maxi = i;				/* max index in client[] array */
			if (--nready <= 0)
				continue;				/* no more readable descriptors */
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i]) == -1)
				continue;
			if (FD_ISSET(sockfd, &rset)) {
				if ( (n = read(sockfd, buf, sizeof(buf))) == -1) {
					perror("read");
					exit(EXIT_FAILURE);
				} else if (n == 0) {
					/* 4connection closed by client */
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				} else {
					if (write(sockfd, buf, n) == -1) {
						perror("write");
						exit(EXIT_FAILURE);
					}
				}

				if (--nready <= 0)
					break;		/* no more readable descriptors */
			}
		}
	}
}
