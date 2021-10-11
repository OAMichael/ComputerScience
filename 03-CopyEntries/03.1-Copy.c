#define BLCSZ 4096
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>


ssize_t writeall(int fd, const void* buf, size_t count)
{
    size_t bytes_written = 0;
    const uint8_t* buf_addr = buf;
    while (bytes_written < count)
    {
        ssize_t res = write(fd, buf_addr + bytes_written, count - bytes_written);
        if(res < 0)
        return res;
        bytes_written += (size_t)res;
    }
    return (ssize_t)bytes_written;
}

// Copy function
size_t copying(const int fd1, const int fd2)
{
    ssize_t tst = 1;
    while(tst)
    {
        void* buf = calloc(BLCSZ, sizeof(char));         // allocation 4Kb (blocksize) of memory   
        tst = read(fd1, buf, BLCSZ);                     // reading into this memory (buf)
        if(tst < 0) 
        {
            perror("Failed to read file");
            free(buf);                                   // free memory in case of failure
            return 6;
        }

        if(writeall(fd2, buf, (size_t)tst) < 0)          // writing into second file from buf
        {
            perror("Failure while writing");
            free(buf);                                   // free memory in case of failure
            return 7;
        }
        free(buf);
    }

    return 0;
}


int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        // Failure in case of insufficient amount of arguments
        fprintf(stderr, "Insufficient amount of arguments\n");
        return 1;
    }

    struct stat sb;

    if (lstat(argv[1], &sb) == -1) 
    {
       perror("lstat");
       return 2;
    }

    // Failure if file is not type of regular
    if(S_ISREG(sb.st_mode)) 
    {
        fprintf(stderr, "Copied file is not 'Regular' type\n");
        return 3;
    }

    // Opening the first file
    int fd1 = open(argv[1], O_RDONLY);
    if (fd1 < 0) 
    {
        perror("Failed to open copied file");
        return 4;
    }

    // Opening the second file
    int fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd2 < 0) 
    {
        perror("Failed to open file for writing");
        return 5;
    }
    // Invoking copying function
    copying(fd1, fd2);

    // closing both files
    if(close(fd1) < 0)                              
    {
        if(close(fd2) < 0)
        {
            perror("Failure while closing the second file");
            return 9;
        }
        perror("Failure while closing the first file");
        return 8;
    }

    return 0;
}
