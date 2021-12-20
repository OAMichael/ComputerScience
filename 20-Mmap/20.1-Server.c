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

// Function which gets current time
int get_time(char* out_buf, size_t buf_len)
{
    time_t cur_time;
    time(&cur_time);

    struct tm* ttime = localtime(&cur_time);

    char yyyymmdd_hhmmss[sizeof("YYYY-mm-dd HH:MM:SS")];
    char tz_str[sizeof("+hhmm")];

    strftime(yyyymmdd_hhmmss, sizeof(yyyymmdd_hhmmss), "%F %T", ttime);
    strftime(tz_str, sizeof(tz_str), "%z", ttime);

    return snprintf(out_buf, buf_len, "%s  %s UTC", yyyymmdd_hhmmss, tz_str);        
}

int main(void)
{
    struct sigaction received_signals = {};

    received_signals.sa_flags = SA_RESTART;
    received_signals.sa_handler = sig_handler;

    sigaction(SIGINT, &received_signals, NULL);
    sigaction(SIGTERM, &received_signals, NULL);

    char time_str[sizeof("YYYY-mm-dd HH:MM:SS  +hhmm UTC")];

    // open shared memory
    int shm_fd = shm_open("/clock", O_RDWR | O_CREAT, 0777);
    if (shm_fd == -1) 
    {
        perror("shm_open");
        return -1;
    }

    // truncating to pagesize
    long page_size = sysconf(_SC_PAGESIZE);
    if(ftruncate(shm_fd, (off_t)page_size) < 0)
    {
        perror("ftruncate");
        close(shm_fd);
        shm_unlink("/clock");
    }

    // mapping to THIS address space
    char* shm_time = (char*)mmap(NULL, sizeof("YYYY-mm-dd HH:MM:SS  +hhmm UTC"), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
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
    sem_t* sem = sem_open(semaphore_name, O_CREAT, 0777, 1);
    if(sem == SEM_FAILED)
    {
        perror("sem_open");
        return -1;
    }

    // main loop
    while(1)
    {
        // if one of signals SIGTERM or SIGINT has been caught, then exit
        if(errno == EINTR && (g_last_signal == SIGTERM || g_last_signal == SIGINT))
        {
            printf("Exiting...\n");
            break;
        }

        // locking the semaphore
        sem_wait(sem);

        get_time(time_str, sizeof(time_str));
        memcpy(shm_time, time_str, strlen(time_str));
        
        // and posting it back
        sem_post(sem);
        sleep(1);
    }

    // clean up after work is done
    shm_unlink("/clock");
    sem_close(sem);
    sem_unlink(semaphore_name);

    return 0;
}