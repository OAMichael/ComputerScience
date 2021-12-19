#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

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


int main(void)
{
    int result = 0;

    struct sigaction recieved_signals = {};

    recieved_signals.sa_flags = SA_SIGINFO | SA_RESTART;
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
    
    while(1)
    {
        // Using pause(), because sigwaitinfo() does not allow to use signal handler
        pause();
        printf("Signal %d (%s) came from process with PID=%d (UID=%d)\n", g_last_signal, strsignal(g_last_signal), g_from_who.si_pid, g_from_who.si_uid);
    }

    return result;
}


