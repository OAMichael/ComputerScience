#ifndef DIR_MODE
#define DIR_MODE 0777
#endif

ssize_t writeall(int fd, const void* buf, size_t count);

ssize_t copying(const int fd1, const int fd2, struct stat* sb);

ssize_t crt_link(const char* pathname, const char* newname);

ssize_t crt_linkat(const int old_fd, const int new_fd, const char* pathname, const char* newname);

char dtype_char(unsigned dtype);

char mode_char(unsigned mode);

ssize_t copy_dir(DIR* old_dir, DIR* new_dir);