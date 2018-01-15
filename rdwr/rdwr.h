#ifndef _CLI_SRV_RDWR_H
#define _CLI_SRV_RDWR_H

#include <sys/types.h>

extern ssize_t readn(int fd, void *buf, size_t nbytes);
extern ssize_t writen(int fd, const void *buf, size_t nbytes);
extern ssize_t readline(int fd, void *buf, size_t line_max);
extern ssize_t readlinebuf(void **bufptr);

#endif /* _CLI_SRV_RDWR_H */
