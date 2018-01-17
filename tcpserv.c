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
#include <poll.h>
#include <sys/stropts.h>
#include "cli_serv.h"

#define LISTENQ 		1024	/* 2nd argument to listen() */
#define OPEN_MAX 		1024

int main(int argc, char **argv)
{
	int 			i, maxi, listenfd, connfd, sockfd;
	int 			nready;
	ssize_t n;
	char buf[LINE_MAX], addr_str[INET_ADDRSTRLEN];
	socklen_t clilen;
	struct pollfd client[OPEN_MAX];
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

	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;
	for (i = 1; i < OPEN_MAX; i++) {
		client[i].fd = -1;		/* -1 indicates available entry */
	}
	maxi = 0;		/* max index into client[] array */
	
	while (1) {
		if ( (nready = poll(client, maxi + 1, -1)) == -1) {
			perror("poll");
			exit(EXIT_FAILURE);
		}
		if (client[0].revents & POLLRDNORM) {	/* new client connection */
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
			for (i = 1; i < OPEN_MAX; i++) {
				if (client[i].fd == -1) {
					client[i].fd = connfd;	/* save descriptor */
					break;
				}
			}
			if (i == OPEN_MAX) {
				fprintf(stderr, "too many clients\n");
				exit(EXIT_FAILURE);
			}
			client[i].events = POLLRDNORM;
			if (i > maxi)
				maxi = i;		/* max index in client[] array */
			if (--nready <= 0)
				continue;		/* no more readable descriptors */
		}

		for (i = 1; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i].fd) == -1)
				continue;
			if (client[i].events & (POLLRDNORM | POLLERR)) {
				if ( (n = read(sockfd, buf, sizeof(buf))) == -1) {
					if (errno == ECONNRESET) {
						/* connection reset by client */
						close(sockfd);
						client[i].fd = -1;
					} else {
						perror("read");
						exit(EXIT_FAILURE);
					}
				} else if (n == 0) {
					/* 4connection closed by client */
					close(sockfd);
					client[i].fd = -1;
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
