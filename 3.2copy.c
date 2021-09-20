#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

ssize_t pwriteall(int fd, const void* buf, size_t count, int offset);
void copying(const char* first, const char* second);                   // copy function


int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		fprintf(stderr, "Insufficient amount of arguments\n");         // Failure in case of insufficient amount of arguments
		exit(EXIT_FAILURE);
	}

	struct stat sb;

	if (lstat(argv[1], &sb) == -1) {
       perror("lstat");
       exit(EXIT_FAILURE);
    }


    if((sb.st_mode & S_IFMT) != S_IFREG) {								// Failure if file is not type of regular
    	fprintf(stderr, "File %s is not 'Regular' type\n", argv[1]);
    	exit(EXIT_FAILURE);
    }

	copying(argv[1], argv[2]);											// invoking copying function
	
	return 0;
}


void copying(const char* first, const char* second)
{

	int fd1, fd2;
	fd1 = open(first, O_RDONLY);										// Opening the first file
	if (fd1 < 0) 
	{
		perror("Failed to open copied file");
		exit(EXIT_FAILURE);
	}

	fd2 = open(second, O_WRONLY | O_CREAT | O_TRUNC, 0644);				// Opening the second file
	if (fd2 < 0) 
	{
		perror("Failed to open file for writing");
		exit(EXIT_FAILURE);
	}

	int offset = 0;
	ssize_t tst = 1;
	while(tst)
	{
		void* buf = calloc(4096, sizeof(char));    						// allocation 4Kb (blocksize) of memory
		tst = pread(fd1, buf, 4096, offset);               				// reading into this memory (buf)
		if(tst < 0) {
			perror("Failed to read file");
			free(buf);													// free memory in case of failure
			exit(EXIT_FAILURE);
		}

		if(pwriteall(fd2, buf, 4096, offset) < 0)          				// writing into second file from buf
		{
			perror("Failure while writing");
			free(buf);													// free memory in case of failure
			exit(EXIT_FAILURE);
		}
		offset += 4096;													// moving offset
		free(buf);
	}

	// closing both files
	if(close(fd1) < 0)
	{
		perror("Failure while closing the first file");
		exit(EXIT_FAILURE);
	}

	if(close(fd2) < 0)
	{
		perror("Failure while closing the second file");
		exit(EXIT_FAILURE);
	}
}


ssize_t pwriteall(int fd, const void* buf, size_t count, int offset)
{
	size_t bytes_written = 0;
	const uint8_t* buf_addr = buf;
	while (bytes_written < count)
	{
		ssize_t res = pwrite(fd, buf_addr + bytes_written, count - bytes_written, offset);
		if(res < 0)
		return res;
		bytes_written += res;
		offset += res;
	}
	return (ssize_t)bytes_written;
}