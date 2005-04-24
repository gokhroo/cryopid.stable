#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "cryopid.h"
#include "cpimage.h"

struct raw_data {
    int fd;
    int mode;
};

static void *raw_init(int fd, int mode) {
    struct raw_data *rd;
    rd = xmalloc(sizeof(struct raw_data));

    rd->fd = fd;
    rd->mode = mode;

    return rd;
}

static void raw_finish(void *fptr) {
    struct raw_data *rd = fptr;
    free(rd);
}

static int raw_read(void *fptr, void *buf, int len) {
    int rlen, togo;
    struct raw_data *rd = fptr;
    char *p;

    togo = len;
    p = buf;
    while (togo > 0) {
	rlen = read(rd->fd, p, len);
	if (rlen <= 0)
	    bail("read(rd->fd, %p, %d) failed: %s", 
		    p, len, strerror(errno));
	p += rlen;
	togo -= rlen;
    }
    return len;
}

static int raw_write(void *fptr, void *buf, int len) {
    int wlen;
    struct raw_data *rd = fptr;

    wlen = write(rd->fd, buf, len);
    return wlen;
}

static void raw_dup2(void *fptr, int newfd) {
    struct raw_data *rd = fptr;

    if (newfd == rd->fd)
	return;

    syscall_check(dup2(rd->fd, newfd), 0, "raw_dup2(%d, %d)", rd->fd, newfd);

    close(rd->fd);
    rd->fd = newfd;
}

struct stream_ops raw_ops = {
    .init = raw_init,
    .read = raw_read,
    .write = raw_write,
    .finish = raw_finish,
    .dup2 = raw_dup2,
};

/* vim:set ts=8 sw=4 noet: */
