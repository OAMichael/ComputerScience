#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

ssize_t pwriteall(int fd, const void* buf, size_t count, int offset);
void copying(const char* first, const char* second);						// copy function
void CrtLink(const char* pathname, const char* newname);					// function which creates a new symlink


int main(int argc, char* argv[])
{
	if(argc != 3) {
		fprintf(stderr, "Insufficient amount of arguments\n");				// Failure in case of insufficient amount of arguments
		exit(EXIT_FAILURE);
	}

	struct stat sb;

	if (lstat(argv[1], &sb) < 0) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    switch(sb.st_mode & S_IFMT) {								// checking file type
	case S_IFREG:	copying(argv[1], argv[2]); break;					// just copying file if original one is regular type	

	case S_IFIFO:	if(mkfifo(argv[2], S_IFIFO) < 0) {					// creating FIFO file if original one is FIFO/pipe
				fprintf(stderr, "Failed to create %s FIFO file\n", argv[2]);
				exit(EXIT_FAILURE);
			} break;
	
	case S_IFLNK:   CrtLink(argv[1], argv[2]); break;					// creating a new link if argv[1] is symlink

	case S_IFBLK:   if(mknod(argv[2], sb.st_mode, sb.st_rdev) < 0) {
				fprintf(stderr, "Failed to create new block device");      	// block device
				exit(EXIT_FAILURE);     
			} break;												

	case S_IFCHR:	if(mknod(argv[2], sb.st_mode, sb.st_rdev) < 0) {
				fprintf(stderr, "Failed to create new character device");      	// character device
				exit(EXIT_FAILURE);     
			} break;

	default: 	fprintf(stderr, "File %s is not type of 'Block device', 'Character device', 'Regular', 'FIFO' or 'Symlink'\n", argv[1]); 
			exit(EXIT_FAILURE);
	}
	return 0;
}


void copying(const char* first, const char* second)
{

	int fd1, fd2;
	fd1 = open(first, O_RDONLY);							// Opening the first file
	if (fd1 < 0) {
		fprintf(stderr, "Failed to open %s\n", first);
		exit(EXIT_FAILURE);
	}

	fd2 = open(second, O_WRONLY | O_CREAT | O_TRUNC, 0644);				// Opening the second file
	if (fd2 < 0) {
		fprintf(stderr, "Failed to open %s for writing\n", second);
		exit(EXIT_FAILURE);
	}

	int offset = 0;
	ssize_t tst = 1;
	while(tst)
	{
		void* buf = calloc(4096, sizeof(char));    				// allocation 4Kb (blocksize) of memory
		tst = pread(fd1, buf, 4096, offset);               			// reading into this memory (buf)
		if(tst < 0) {
			fprintf(stderr, "Failed to read %s\n", first);
			free(buf);							// free memory in case of failure
			exit(EXIT_FAILURE);
		}

		if(pwriteall(fd2, buf, 4096, offset) < 0)          			// writing into second file from buf
		{
			fprintf(stderr, "Failure while writing to %s\n", second);
			free(buf);							// free memory in case of failure
			exit(EXIT_FAILURE);
		}
		offset += 4096;								// moving offset
		free(buf);
	}

	// closing both files
	if(close(fd1) < 0) {
		fprintf(stderr, "Failure while closing %s\n", first);
		exit(EXIT_FAILURE);
	}

	if(close(fd2) < 0) {
		fprintf(stderr, "Failure while closing %s\n", second);
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

void CrtLink(const char* pathname, const char* newname)
{
	char* buf = (char*)calloc(strlen(pathname) + 4, sizeof(char));			// memory allocation

	if(readlink(pathname, buf, strlen(pathname) + 4) < 0) {				// reading the link
		fprintf(stderr, "Failed to read link\n");
		free(buf);								// free memory in case of failure
		exit(EXIT_FAILURE);
	}

	if (symlink(buf, newname) < 0) {                                    		// creating new link
		fprintf(stderr, "Failed to create a  new link\n");
		free(buf);								// free memory in case of failure
		exit(EXIT_FAILURE);
	}
}
