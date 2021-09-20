#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

ssize_t writeall(int fd, const void* buf, size_t count);

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		fprintf(stderr, "Usage: %s filename tex-to-write\n", argv[0]);      // Failure in case of insufficient amount of arguments
		return 1;
	}
	// rw for user, read-only for group and others
	int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);				// opening pointed file
	if (fd < 0) 
	{
		perror("Failed to open file for writing");
		return 2;
	}

	if(writeall(fd, argv[2], strlen(argv[2])) < 0)							// writing entered string into file
	{
		perror("Failure while writing");
		return 3;
	}

	if(close(fd) < 0)
	{
		perror("Failure while closing fd");
		return 4;
	}

	return 0;
}


ssize_t writeall(int fd, const void* buf, size_t count)
{
	size_t bytes_written = 0;
	const uint8_t* buf_addr = buf;
	while (bytes_written < count)
	{
		ssize_t res = write(fd, buf_addr + bytes_written, count - bytes_written);
		if(res < 0)
		return res;
		bytes_written += res;
	}
	return (ssize_t)bytes_written;
}

