#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

void copying(const char* first, const char* second);					// copy function
ssize_t writeall(int fd, const void* buf, size_t count);	


int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		fprintf(stderr, "Insufficient amount of arguments\n");			// Failure in case of insufficient amount of arguments
		exit(EXIT_FAILURE);
	}

	copying(argv[1], argv[2]);

	return 0;
}


void copying(const char* first, const char* second)
{
	struct stat sb;
	if (lstat(first, &sb) < 0) {
       perror("lstat");
       exit(EXIT_FAILURE);
    }

    if((sb.st_mode & S_IFMT) != S_IFREG) {
    	fprintf(stderr, "Copied file is not 'Regular' type\n");				// Failure if file is not type of regular
    	exit(EXIT_FAILURE);
    }

	int fd1, fd2;
	fd1 = open(first, O_RDONLY);							// Opening the first file
	if (fd1 < 0) 
	{
		perror("Failed to open copied file");
		exit(EXIT_FAILURE);
	}

	fd2 = open(second, O_WRONLY | O_CREAT | O_TRUNC, 0600);				// Opening the second file
	if (fd2 < 0) 
	{
		perror("Failed to open file for writing");
		exit(EXIT_FAILURE);
	}


	ssize_t tst = 1;
	while(tst)
	{
		void* buf = calloc(4096, sizeof(char));    				// allocation 4Kb (blocksize) of memory
		tst = read(fd1, buf, 4096);               				// reading into this memory (buf)
		if(tst < 0) {
			perror("Failed to read file");
			free(buf);							// free memory in case of failure
			exit(EXIT_FAILURE);
		}

		if(writeall(fd2, buf, 4096) < 0)          				// writing into second file from buf
		{
			perror("Failure while writing");
			free(buf);							// free memory in case of failure
			exit(EXIT_FAILURE);
		}
		free(buf);
	}

	if(fchown(fd2, sb.st_uid, sb.st_gid) < 0) {					// copying User Id and Group Id
		perror("Failure while copying UID and GID");
		exit(EXIT_FAILURE);
	}


	if(fchmod(fd2, sb.st_mode) < 0) {						// copying access permissions
		perror("Failure while copying access permissions");
		exit(EXIT_FAILURE);
	}

	struct timespec times[2];							// creating timespec structure
	times[0].tv_sec = sb.st_atim.tv_sec;						// for futimens(int fd, struct timespec times)
	times[0].tv_nsec = sb.st_atim.tv_nsec;
	times[1].tv_sec = sb.st_mtim.tv_sec;
	times[1].tv_nsec = sb.st_mtim.tv_nsec;

	if(futimens(fd2, times) < 0) {							// copying atime and mtime of file
		perror("Failure while copying times");
		exit(EXIT_FAILURE);
	}

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
