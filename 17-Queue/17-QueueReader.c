#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <mqueue.h>
#include <signal.h>

volatile int g_last_signal;
volatile siginfo_t g_from_who;

void sig_handler(int signum, siginfo_t* info, void* ucontext)
{
    g_last_signal = signum;
    g_from_who.si_pid = info->si_pid;
    g_from_who.si_uid = info->si_uid;
    
    // Just because it is useless for this program
    ucontext = (void*)ucontext;
}

int main(int argc, char* argv[])
{
    int result = 0;

    if(argc != 2)
    {
        printf("Usage: %s /queuename\n", argv[0]);
        return 1;
    }

    struct sigaction recieved_signals = {};

    // No flag SA_RESTART to watch received signals
    recieved_signals.sa_flags = SA_SIGINFO;
    recieved_signals.sa_sigaction = sig_handler;

    for(int i = 1; i < NSIG; i++)
    {
        if(i == SIGKILL || i == SIGSTOP)
            continue;

        if(sigaction(i, &recieved_signals, NULL) < 0)
        {
            perror("sigaction");
            result = -1;
        }
    }

    // create a new queue 
    mqd_t queue = mq_open(argv[1], O_RDWR | O_CREAT | O_EXCL, 0600, NULL);

    if(queue == (mqd_t) -1)
    {
        perror("mq_open");
        return 1;
    }

    // get and show queue info
    struct mq_attr queue_info = {};
    mq_getattr(queue, &queue_info);

    // preparing to read messages
    char message[queue_info.mq_msgsize];
    memset(message, 0, sizeof(message));

    printf("Messages:\n");
    while(1)
    {
        unsigned msg_prio;
        ssize_t msg_len = mq_receive(queue, message, sizeof(message), &msg_prio);
        if(msg_len == -1)
        {
            if(errno == EINTR)
            {
            printf("The signal %d (%s) was sent from PID=%d (UID=%d)\n", g_last_signal, strsignal(g_last_signal), g_from_who.si_pid, g_from_who.si_uid);

                if(g_last_signal == SIGTERM)
                {
                    printf("Terminating...\n");
                    break;
                }

                if(g_last_signal == SIGQUIT)
                {
                    printf("Exiting...\n");
                    break;
                }
                
                errno = 0;
                continue;
            }
        }
        printf("'%.*s'\n", (int)msg_len, message);
    }

    // clean up
    mq_unlink(argv[1]);
    mq_close(queue);

    return result;
}
