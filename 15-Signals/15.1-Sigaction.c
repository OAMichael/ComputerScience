#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define NUMBER_OF_SIGNALS 32


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

    const int signals[NUMBER_OF_SIGNALS] = 
    {
        SIGABRT,
        SIGALRM,
        SIGBUS,
        SIGCHLD,
        SIGCLD,
        SIGCONT,
        SIGFPE,
        SIGHUP,
        SIGILL,
        SIGINT,
        SIGIO,
        SIGIOT,
        SIGPIPE,
        SIGPOLL,
        SIGPROF,
        SIGPWR,
        SIGQUIT,
        SIGSEGV,
        SIGSTKFLT,
        SIGTSTP,
        SIGSYS,
        SIGTERM,
        SIGTRAP,
        SIGTTIN,
        SIGTTOU,
        SIGURG,
        SIGUSR1,
        SIGUSR2,
        SIGVTALRM,
        SIGXCPU,
        SIGXFSZ,
        SIGWINCH,
    };

    struct sigaction recieved_signals = {};

    recieved_signals.sa_flags = SA_SIGINFO;
    recieved_signals.sa_sigaction = sig_handler;


    for(int i = 0; i < NUMBER_OF_SIGNALS; i++)
    {
        if(sigaction(signals[i], &recieved_signals, NULL) < 0)
        {
            perror("sigaction");
            result = -1;
        }
    }
    
    while(1)
    {
        pause();
        printf("Signal %d(%s) came from process with PID=%d (UID=%d)\n", g_last_signal, strsignal(g_last_signal), g_from_who->si_pid, g_from_who->si_uid);
    }

    return result;
}


