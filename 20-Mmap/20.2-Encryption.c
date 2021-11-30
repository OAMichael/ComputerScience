#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char* argv[])
{
    char key = 'a';

    if(argc < 2)
    {
        printf("Usage: %s <file> [key]\n", argv[0]);
        return 1;
    }

    if(argc == 3)
        key = argv[2][0];

    int fd = open(argv[1], O_RDWR);
    if(fd < 0)
    {
        perror("open");
        return 2;
    }

    struct stat sb;

    fstat(fd, &sb);

    char* data = mmap(NULL, (size_t)sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(data == MAP_FAILED)
    {
        perror("mmap");
        return 3;
    }

    for(unsigned i = 0; i < sb.st_size; i++)
        data[i] ^= key;

    munmap(data, (size_t)sb.st_size);
    close(fd);

    puts("Work is done!");

    return 0;
}
