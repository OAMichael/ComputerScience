#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <mqueue.h>
#include <signal.h>
#include "../Library/util.h"


volatile int g_last_signal = -1;
volatile siginfo_t* g_from_who;

void sig_handler(int signum, siginfo_t* info, void* ucontext)
{
    g_last_signal = signum;
    g_from_who = info;

    // Just because it is useless for this program
    if(ucontext != NULL)
        ucontext = NULL;
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

    recieved_signals.sa_flags = SA_SIGINFO;
    recieved_signals.sa_sigaction = sig_handler;

    // __signals__ and NUMBER_OF_SIGNALS defined in util.h
    for(int i = 0; i < NUMBER_OF_SIGNALS; i++)
    {
        if(sigaction(__signals__[i], &recieved_signals, NULL) < 0)
        {
            perror("sigaction");
            result = -1;
        }
    }

    // create a new queue 
    mqd_t queue = mq_open(argv[1], O_RDWR | O_CREAT, 0600, NULL);

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
        if(g_last_signal != -1)
        {
            printf("The signal %d (%s) was sent from PID=%d\n", g_last_signal, strsignal(g_last_signal), g_from_who->si_pid);

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
            
            g_last_signal = -1;
        }

        mq_getattr(queue, &queue_info);
        if(queue_info.mq_curmsgs > 0)
        {
            mq_receive(queue, message, sizeof(message), NULL);
            printf("'%s'\n", message);
            memset(message, 0, sizeof(message));
        }
    }

    // clean up
    mq_unlink(argv[1]);
    mq_close(queue);


    return result;
}
