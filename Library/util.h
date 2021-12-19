#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <sys/param.h>


#define ALL_PERMS 0777
#define BLCSZ DEV_BSIZE

#ifndef SYMLINK_MAX
#define SYMLINK_MAX _POSIX_SYMLINK_MAX
#endif

ssize_t writeall(int fd, const void* buf, size_t count);

ssize_t pwriteall(int fd, const void* buf, size_t count, off_t offset);

int copying_entry(const int fd1, const int fd2);

int copying_all(const int fd1, const int fd2, const struct stat* sb);

int pcopying(const int fd1, const int fd2);

int crt_link(const char* pathname, const char* newname);

int crt_linkat(const int old_fd, const int new_fd, const char* pathname, const char* newname);

char dtype_char(unsigned dtype);

char mode_char(unsigned mode);

int copy_dir(DIR* old_dir, DIR* new_dir);


#endif      /*  UTIL_H  */
