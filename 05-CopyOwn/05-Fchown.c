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
    int result = 0;

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
        perror("open");
        return 4;
    }

    // Opening the second file
    fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, ALL_PERMS);
    if (fd2 < 0) 
    {
        perror("open");
        return 5;
    }

    if(copying_all(fd1, fd2, &sb) < 0)
    {
        perror("copying_all");
        result = 6;
    }

    // Closing both files
    if(close(fd1) < 0)
    {
        perror("close");
        result = 7;
    }

    if(close(fd2) < 0)
    {
        perror("close");
        result = 8;
    }

    return result;
}
