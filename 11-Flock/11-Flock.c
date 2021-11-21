#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

//#define TEST_MULTIPROG


int main(void)
{
    const char* file_name = "FILE_COUNTER";
    int fd = 0;
    int counter = 1;
    
    char buf[sizeof("99999999999")];         // Up to 99999999999 of openings
    memset(buf, 0, sizeof(buf));
    
    fd = open(file_name, O_RDWR | O_CREAT, 0777);

    // Locking file for reading and writing
    lockf(fd, F_LOCK, 0);

    // The code executed only while file locked by THIS program

    long int bytes_read = read(fd, buf, sizeof(buf));
    if(bytes_read < 0)
    {
        perror("read");
        lockf(fd, F_ULOCK, 0);
        close(fd);
        return -1;
    }

// Just to demostrate how program works with other copies of itself
#ifdef TEST_MULTIPROG
    
    sleep(5);

#endif


    char number[bytes_read > 0 ? (bytes_read + 1): 2];
    memset(number, 0, sizeof(number));

    counter += atoi(buf);

    sprintf(number, "%d\n", counter);

    if(pwrite(fd, number, sizeof(number), 0) < 0)
    {
        perror("write");
        lockf(fd, F_ULOCK, 0);
        close(fd);
        return -1;
    }

    // Unlocking and closing file after work is done
    lockf(fd, F_ULOCK, 0);
    close(fd);


    return 0;
}