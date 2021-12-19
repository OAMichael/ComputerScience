#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <poll.h>
#include <sys/syscall.h>


#ifndef __NR_pidfd_open
#define __NR_pidfd_open 434   /* System call # on most architectures */
#endif

static int pidfd_open(pid_t pid, unsigned int flags)
{
   return (int)syscall(__NR_pidfd_open, pid, flags);
}



void proc_info(const char* procname)
{
    printf("%s: PID %d, PPID %d, PGID %d, SID %d\n", procname, getpid(), getppid(), getpgid(0), getsid(0));
}


int main(void)
{
    pid_t parent_pid = getpid();
    pid_t child_id = fork();
    if(child_id < 0)
    {
        perror("fork");
        return 1;
    }

    if(child_id == 0)
    {
        /* this code is executed in child process only */
        proc_info("Child");

        int pidfd = pidfd_open(parent_pid, 0);

        struct pollfd pollfd = {
            .fd = pidfd,
            .events = POLLIN
        };

        int poll_num = poll(&pollfd, 1, -1);
        if(poll_num < 0)
        {
            perror("poll");
            return -1;
        }

        printf("Parent process (PID=%d) has ended its execution\n", parent_pid);        

        return 0;
    }
    else
    {
        /* this code is executed in parent process only */
        proc_info("Parent");

        sleep(20);
    } 
    return 0; 
}