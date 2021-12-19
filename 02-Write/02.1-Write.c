#include <string.h>
#include <stdio.h>
#include <stdint.h>
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
        fprintf(stderr, "Usage: %s filename text-to-write\n", argv[0]);
        return 1;
    }

    // Opening pointed file. rw for user, read-only for group and others
    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644); 
    if (fd < 0) 
    {
        perror("Failed to open file for writing");
        return 2;
    }

    // Writing entered string into file
    if(writeall(fd, argv[2], strlen(argv[2])) < 0)
        perror("Failure while writing");

    if(close(fd) < 0)
    {
        perror("Failure while closing fd");
        return 4;
    }

    return 0;
}

