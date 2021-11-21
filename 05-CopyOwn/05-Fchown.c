#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

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
    if (lstat(argv[1], &sb) < 0) 
    {
       perror("lstat");
       return 2;
    }

    // Failure if file is not type of regular
    if(!S_ISREG(sb.st_mode)) 
    {
        fprintf(stderr, "Copied file is not 'Regular' type\n");
        return 3;
    }

    int fd1 = 0, fd2 = 0;

    // Opening the first file
    fd1 = open(argv[1], O_RDONLY);
    if (fd1 < 0) 
    {
        perror("Failed to open copied file");
        return 4;
    }

    // Opening the second file
    fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd2 < 0) 
    {
        perror("Failed to open file for writing");
        return 5;
    }


    copying(fd1, fd2, &sb);

    // Closing both files
    if(close(fd1) < 0)
    {
        perror("Failure while closing the first file");
        
        if(close(fd2) < 0)
        {
            perror("Failure while closing the second file");
            return 11;
        }
        return 10;
    }
    return 0;
}
