#include <stdio.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


// I could not find out a way to get something meaningful
// and could not find out what particular system calls needed
// to obtain information which 'filefrag' and 'hdparm --fibmap' do 
int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);
    if(fd < 0)
    {
        perror("open");
        return -1;
    }

    int result = 0;
    int fib = 0;

    if(ioctl(fd, FIBMAP, &fib) < 0)
    {
        perror("ioctl");
        result = -1;
    }

    printf("Block number in the filesystem: %d\n", fib);

    if(close(fd) < 0)
    {
        perror("close");
        result = -1;
    }

    return result;
}