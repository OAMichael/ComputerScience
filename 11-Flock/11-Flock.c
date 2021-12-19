#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>

#define ALL_PERMS 0777

//#define TEST_MULTIPROG


int main(void)
{
    // Independent path to file-counter
    struct passwd* pw = getpwuid(getuid());
    char* file_name = calloc(PATH_MAX, sizeof(char));
    if(!file_name)
    {
        perror("calloc");
        return -1;
    }
    file_name = pw->pw_dir;
    strcat(file_name, "/FLOCK_FILE_COUNTER.txt");
    
    int fd = 0;
    int counter = 1;
    
    char buf[sizeof("99999999999")];         // Up to 99999999999 openings
    memset(buf, 0, sizeof(buf));
    
    fd = open(file_name, O_RDWR | O_CREAT, ALL_PERMS);

    if(fd < 0)
    {
        perror("open");
        return -1;
    }

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

    printf("The program was executed %d time(s)\n", counter);
    sprintf(number, "%d\n", counter);

    if(pwrite(fd, number, sizeof(number), 0) < 0)
    {
        perror("pwrite");
        lockf(fd, F_ULOCK, 0);
        close(fd);
        return -1;
    }

    // Unlocking and closing file after work is done
    lockf(fd, F_ULOCK, 0);
    close(fd);


    return 0;
}
