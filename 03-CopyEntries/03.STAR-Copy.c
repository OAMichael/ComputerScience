#define BLCSZ 4096
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <limits.h>


ssize_t pwriteall(int fd, const void* buf, size_t count, long int offset)
{
    size_t bytes_written = 0;
    const uint8_t* buf_addr = buf;
    while (bytes_written < count)
    {
        ssize_t res = pwrite(fd, buf_addr + bytes_written, count - bytes_written, offset);
        if(res < 0)
            return res;
        bytes_written += (size_t)res;
        offset += (long int)res;
    }
    return (ssize_t)bytes_written;
}

// Copy function
size_t copying(const int fd1, const int fd2)
{

    off_t offset = 0;
    ssize_t tst = 1;
    while(tst)
    {
        void* buf = calloc(BLCSZ, sizeof(char));                         // allocation 4Kb (blocksize) of memory
        tst = pread(fd1, buf, BLCSZ, offset);                            // reading into this memory (buf)
        if(tst < 0) 
        {
            perror("Failed to read file");
            free(buf);                                                   // free memory in case of failure
            return 8;
        }

        // Writing into second file from buf
        if(pwriteall(fd2, buf, (size_t)tst, offset) < 0)
        {
            perror("Failure while writing");
            free(buf);                                                   // free memory in case of failure
            return 9;
        }
        offset += (off_t)tst;                                            // moving offset
        free(buf);
    }

    return 0;
}

// Function which creates a new symlink
size_t crt_link(const char* pathname, const char* newname)
{
    // memory allocation
    char* buf = (char*)calloc(PATH_MAX, sizeof(char));
    size_t result = 0;

    // reading the link
    if(readlink(pathname, buf, PATH_MAX) < 0) 
    {
        fprintf(stderr, "Failed to read link\n");
        result = 12;
    }

    // creating new link
    if (symlink(buf, newname) < 0) 
    {
        fprintf(stderr, "Failed to create a  new link\n");               
        result = 13;
    }

    free(buf);  
    return result;
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

    if (lstat(argv[1], &sb) < 0) 
    {
       perror("lstat");
       return 2;
    }

    // Checking file type
    switch(sb.st_mode & S_IFMT) 
    {

        // Just copying file if original one is regular type
        case S_IFREG:  
        {
            // Opening the first file
            int fd1 = open(argv[1], O_RDONLY);
            if (fd1 < 0) 
            {
                fprintf(stderr, "Failed to open %s\n", argv[1]);
                return 6;
            }

            // Opening the second file
            int fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd2 < 0) 
            {
                fprintf(stderr, "Failed to open %s for writing\n", argv[2]);
                return 7;
            }

            copying(fd1, fd2);

            // closing both files
            if(close(fd1) < 0) 
            {
                fprintf(stderr, "Failure while closing %s\n", argv[1]);

                if(close(fd2) < 0) 
                {
                    fprintf(stderr, "Failure while closing %s\n", argv[2]);
                    return 11;
                } 
                return 10;
            }

            break;   
        }
        // Creating FIFO file if original one is FIFO/pipe
        case S_IFIFO:   if(mkfifo(argv[2], sb.st_mode) < 0) 
                        {
                            fprintf(stderr, "Failed to create %s FIFO file\n", argv[2]);
                            return 3;
                        }   break;
        
        // Creating a new link if argv[1] is symlink
        case S_IFLNK:   crt_link(argv[1], argv[2]);  break;

        // Block device
        case S_IFBLK:                                               
        // Character device
        case S_IFCHR:   if(mknod(argv[2], sb.st_mode, sb.st_rdev) < 0) 
                        {
                            fprintf(stderr, "Failed to create new character device");
                            return 4;     
                        }   break;

        default:        fprintf(stderr, "File %s is not type of 'Block device', 'Character device', 'Regular', 'FIFO' or 'Symlink'\n", argv[1]); 
                        return 5;
    }
    return 0;
}
