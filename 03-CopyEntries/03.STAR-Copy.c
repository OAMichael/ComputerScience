#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include "../Library/util.h"


int main(int argc, char* argv[])
{
    if(argc != 3) 
    {
        // Failure in case of insufficient amount of arguments
        fprintf(stderr, "Insufficient amount of arguments\n");
        return 1;
    }

    struct stat sb;
    int result = 0;

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

            if(pcopying(fd1, fd2) < 0)
            {
                perror("pcopying");
                result = 8;
            }

            // closing both files
            if(close(fd1) < 0) 
            {
                perror("close");
                result = 9;
            }
            if(close(fd2) < 0) 
            {
                perror("close");
                result = 10;
            }    

            break;
        }

        // Creating FIFO file if original one is FIFO/pipe
        case S_IFIFO:
        {
            if(mkfifo(argv[2], sb.st_mode) < 0) 
            {
                fprintf(stderr, "Failed to create %s FIFO file\n", argv[2]);
                return 3;
            }   
            break;
        }
        // Creating a new link if argv[1] is symlink
        case S_IFLNK:
        {   
            if(crt_link(argv[1], argv[2]) < 0)
            {
                fprintf(stderr, "Failed to create %s symlink\n", argv[2]);
                return 4;
            }  
            break;
        }
        // Block device
        case S_IFBLK:                                               
        // Character device
        case S_IFCHR:   
        {
            if(mknod(argv[2], sb.st_mode, sb.st_rdev) < 0) 
            {
                fprintf(stderr, "Failed to create new block/character device");
                return 5;     
            }   
            break;
        }

        default:        
        {
            fprintf(stderr, "File %s is not type of 'Block device', 'Character device', 'Regular', 'FIFO' or 'Symlink'\n", argv[1]); 
            return 6;
        }
    }

    return result;
}
