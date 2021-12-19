#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        printf("Usage: %s <signal> <PID> [message]\n", argv[0]);
        return -1;
    }

    // These two arrays left unchanged because strsignal() and sys_siglist[NSIG]
    // returns/contains description of a signal instead of canonical signal name
    // For example, strsignal(SIGKILL) returns "Killed" instead of "SIGKILL". 
    // sys_siglist does the same thing
    const int signals[NSIG] = 
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
        SIGKILL,
        SIGSTOP
    };

    const char* str_signals[NSIG] = 
    {
        "SIGABRT",
        "SIGALRM",
        "SIGBUS",
        "SIGCHLD",
        "SIGCLD",
        "SIGCONT",
        "SIGFPE",
        "SIGHUP",
        "SIGILL",
        "SIGINT",
        "SIGIO",
        "SIGIOT",
        "SIGPIPE",
        "SIGPOLL",
        "SIGPROF",
        "SIGPWR",
        "SIGQUIT",
        "SIGSEGV",
        "SIGSTKFLT",
        "SIGTSTP",
        "SIGSYS",
        "SIGTERM",
        "SIGTRAP",
        "SIGTTIN",
        "SIGTTOU",
        "SIGURG",
        "SIGUSR1",
        "SIGUSR2",
        "SIGVTALRM",
        "SIGXCPU",
        "SIGXFSZ",
        "SIGWINCH",
        "SIGKILL",
        "SIGSTOP"
    };

    int sending_signal = -1;

    for(int i = 0; i < NSIG; i++)
        if(!strcmp(argv[1], str_signals[i]))
        {
            sending_signal = signals[i];
            break;
        }

    if(sending_signal < 0)
    {
        printf("There is no such signal: %s\n", argv[1]);
        return -1;
    }


    pid_t proc_id = atoi(argv[2]);

    int message = 0;

    if(argc > 3)
    {
        message = atoi(argv[3]);
    }

    const union sigval value = {.sival_int = message};

    if(sigqueue(proc_id, sending_signal, value) < 0)
    {
        perror("sigqueue");
        return -1;
    }

    return 0;
}
