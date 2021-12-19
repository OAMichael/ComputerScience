#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/ptrace.h>


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

        if(ptrace(PTRACE_SEIZE, parent_pid, 0, 0) < 0)
        {
            perror("ptrace");
            return -1;
        }

        int status = 0;
        pid_t res = 0;

        while(1) 
        {
            res = waitpid(-1, &status, WCONTINUED | WSTOPPED);
            if(res < 0)
            {
                perror("waitpid");
                return -1;
            }
            
            if(WIFEXITED(status))
            {
                printf("Parent (PID=%d) has ended its execution\n", parent_pid);  
                break;
            }

            if(WIFSTOPPED(status))
            {
                printf("Parent (PID=%d) has caught a signal %d (%s)\n", res, WSTOPSIG(status), strsignal(WSTOPSIG(status)));
                ptrace(PTRACE_CONT, parent_pid, 0, 0);
            }
        }        
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