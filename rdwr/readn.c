#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include "rdwr.h"

ssize_t readn(int fd, void *buf, size_t nbytes)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;

	ptr = (char *)buf;
	nleft = nbytes;
	while (0 < nleft) {
		if ( (nread = read(fd, ptr, nleft)) == -1) {
			if (errno == EINTR) {
				nread = 0;	/* and call read() again */
			} else {
				return -1;
			}
		} else if (nleft == 0) {
			break;				/* EOF */
		}

		nleft -= nread;
		ptr += nread;
	}
	return (nbytes - nleft);			/* return >= 0 */
}
