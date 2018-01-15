#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include "rdwr.h"

ssize_t writen(int fd, const void *buf, size_t nbytes)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = (const char *)buf;
	nleft = nbytes;
	while (0 < nleft) {
		if ( (nwritten = write(fd, ptr, nleft)) == -1) {
			if (errno == EINTR) {
				nwritten = 0;
			} else {
				return -1;
			}
		} else if (nwritten == 0) {
			return -1;		/* 0 means error in block mode */
		}

		nleft -= nwritten;
		ptr += nwritten;
	}
	return nbytes;
}
