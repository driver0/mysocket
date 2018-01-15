#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <errno.h>
#include "rdwr.h"

static int read_cnt;
static char *read_ptr;
static char read_buf[LINE_MAX];

/* read one character peer time with user-buffered io */
static ssize_t buffered_read(int fd, char *ptr)
{
	if (read_cnt <= 0) {
	again:
		if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) == -1) {
			if (errno == EINTR) {
				goto again;
			} else {
				return -1;
			}
		} else if (read_cnt == 0) {
			return 0;
		}
		read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;
	return 1;
}

ssize_t readline(int fd, void *buf, size_t line_max)
{
	size_t n;
	ssize_t rc;
	char c, *ptr;

	ptr = (char *)buf;
	for (n = 1; n < line_max; n++) {
		if ( (rc = buffered_read(fd, &c)) == 1) {
			*ptr++ = c;
			if (c == '\n') {
				break;			/* newline is stored, like fgets() */
			}
		} else if (rc == 0) {
			*ptr = 0;
			return n - 1;		/* EOF, n - 1 bytes were read */
		} else {
			return -1;			/* error, errno set by read() */
		}
	}

	*ptr = 0;					/* null terminate like fgets() */
	return n;
}

ssize_t readlinebuf(void **bufptr) {
	if (read_cnt)
		*bufptr = read_ptr;
	return read_cnt;
}
