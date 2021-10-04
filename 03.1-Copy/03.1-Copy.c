#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

// Copy function
size_t copying(const char* first, const char* second);
ssize_t writeall(int fd, const void* buf, size_t count);


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
    if((sb.st_mode & S_IFMT) != S_IFREG) 
    {
        fprintf(stderr, "Copied file is not 'Regular' type\n");
        return 3;
    }

    // Invoking copying function
    copying(argv[1], argv[2]);

    return 0;
}


size_t copying(const char* first, const char* second)
{
    int fd1, fd2;

    // Opening the first file
    fd1 = open(first, O_RDONLY);
    if (fd1 < 0) 
    {
        perror("Failed to open copied file");
        return 4;
    }

    // Opening the second file
    fd2 = open(second, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd2 < 0) 
    {
        perror("Failed to open file for writing");
        return 5;
    }


    ssize_t tst = 1;
    while(tst)
    {
        void* buf = calloc(4096, sizeof(char));         // allocation 4Kb (blocksize) of memory   
        tst = read(fd1, buf, 4096);                     // reading into this memory (buf)
        if(tst < 0) 
        {
            perror("Failed to read file");
            free(buf);                                  // free memory in case of failure
            return 6;
        }

        if(writeall(fd2, buf, 4096) < 0)                // writing into second file from buf
        {
            perror("Failure while writing");
            free(buf);                                  // free memory in case of failure
            return 7;
        }
        free(buf);
    }

    // closing both files
    if(close(fd1) < 0)                              
    {
        perror("Failure while closing the first file");
        return 8;
    }

    if(close(fd2) < 0)
    {
        perror("Failure while closing the second file");
        return 9;
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