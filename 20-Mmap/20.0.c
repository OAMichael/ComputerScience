#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        printf("Usage: %s /shm-area-name\n", argv[0]);
        return 1;
    }

    int shm_fd = shm_open(argv[1], O_RDWR | O_CREAT, 0600);
    if(shm_fd == -1)
    {
        perror("shm_open");
        return 1;
    }

    long page_size = sysconf(_SC_PAGESIZE);
    if(ftruncate(shm_fd, (off_t)page_size) < 0)
    {
        perror("ftruncate");
        close(shm_fd);
        shm_unlink(argv[1]);
    }

    int* x = mmap(NULL, (size_t)page_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, shm_fd, 0);
    if(x == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }
    close(shm_fd);

    *x = 42;

    printf("parent 1:\t %d\n", *x);
    pid_t child_id = fork();

    if(child_id < 0)
    {
        perror("fork");
        return 1;
    }

    if(child_id == 0)
    {
        // this code executed in the child process only
        printf("child 1:\t %d\n", *x);
        *x = 7500;
        printf("child 2:\t %d\n", *x);
        munmap(x, sizeof(int));
        return 0;
    }

    wait(NULL);
    printf("parent 2:\t %d\n", *x);
    return 0;
}