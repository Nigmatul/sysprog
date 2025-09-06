#ifndef __SYSCALLS_H
#define __SYSCALLS_H

int open(const char *pathname, int flags, mode_t mode);
int close(int fd);

int fstat(int fd, struct stat *statbuf);

ssize_t read(int fildes, void *buf, size_t nbyte);
ssize_t write(int fildes, const void *buf, size_t nbyte);

off_t lseek(int fildes, off_t offset, int whence);

void exit(int status);

#endif // __SYSCALLS_H
