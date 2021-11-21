#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>


void proc_info(const char* procname)
{
    printf("%s: PID %d, PPID %d, PGID %d, SID %d\n", procname, getpid(), getppid(), getpgid(0), getsid(0));
}


int main(int argc, char* argv[])
{
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
		if(argc > 1)
			printf("%d\n", 42 / atoi(argv[1]));

        sleep(20);

		return 42;
	}
	else
	{
		/* this code is executed in parent process only */
		proc_info("Parent");
		int status;
        pid_t res = 0;

        while(1) 
        {
            res = waitpid(-1, &status, WUNTRACED | WCONTINUED);
            
            if(WIFSTOPPED(status))
            {
                printf("Child with PID = %d has been stopped by signal %d (%s)\n", res, WSTOPSIG(status), strsignal(WSTOPSIG(status)));
            }

            if(WIFCONTINUED(status))
            {
                printf("Child with PID = %d has been continued\n", res);
            }

            if(WIFEXITED(status))
            {
                printf("Child with PID = %d has exited normally with code %d\n", res, WEXITSTATUS(status));
                break;
            }
            
            if(WIFSIGNALED(status))
            {
                int child_signal = WTERMSIG(status);
                printf("Child with PID = %d was killed by signal %d (%s)\n", res, child_signal, strsignal(child_signal));
                
                if(WCOREDUMP(status))
                    printf("Child with PID = %d has caused core dump\n", res);

                break;
            }

    	}
    } 
    return 0; 
}
