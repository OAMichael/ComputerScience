#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "../Library/util.h"


volatile int g_last_signal;
volatile siginfo_t* g_from_who;

void sig_handler(int signum, siginfo_t* info, void* ucontext)
{
    g_last_signal = signum;
    g_from_who = info;

    // Just because it is useless for this program
    if(ucontext != NULL)
        ucontext = NULL;
}


int main(void)
{
    int result = 0;

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
    
    while(1)
    {
        pause();
        printf("Signal %d (%s) came from process with PID=%d (UID=%d)\n", g_last_signal, strsignal(g_last_signal), g_from_who->si_pid, g_from_who->si_uid);
    }

    return result;
}


