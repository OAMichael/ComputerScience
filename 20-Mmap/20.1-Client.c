#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <time.h>

volatile int g_last_signal;

void sig_handler(int signum)
{
    g_last_signal = signum;
}

int main(void)
{
    struct sigaction received_signals = {};

    received_signals.sa_flags = SA_RESTART;
    received_signals.sa_handler = sig_handler;

    sigaction(SIGINT, &received_signals, NULL);
    sigaction(SIGTERM, &received_signals, NULL);

    // open shared memory with O_RDONLY
    int shm_fd = shm_open("/clock", O_RDONLY, 0);
    if (shm_fd == -1) 
    {
        perror("shm_open");
        return -1;
    }

    // mapping to current address space
    char* shm_time = (char*)mmap(NULL, sizeof("YYYY-mm-dd HH:MM:SS  +hhmm UTC"), PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm_time == MAP_FAILED) 
    {
        perror("mmap");
        return -1;
    }

    // closing descriptor,
    // it is useless already
    if(close(shm_fd) < 0)
    {
        perror("close");
        return -1;
    }

    // opening named semaphore
    const char* semaphore_name = "semaphore";
    sem_t* sem = sem_open(semaphore_name, O_EXCL);
    if(sem == SEM_FAILED)
    {
        perror("sem_open");
        return -1;
    }

    while(1)
    {
        // checking if SIGTERM of SIGINT has been caught
        if(errno == EINTR && (g_last_signal == SIGTERM || g_last_signal == SIGINT))
        {
            printf("Exiting...\n");
            break;
        }

        // hold semaphore
        if(sem_wait(sem) < 0)
        {
            perror("sem_wait");
            sem_post(sem);
            break;
        }

        // printing the time
        printf("%s\n", shm_time);
        
        // unlocking the semaphore
        sem_post(sem);
        sleep(1);
    }

    // close semaphore
    sem_close(sem);

    return 0;
}