#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/signalfd.h>
#include "../Library/util.h"


int main(void)
{
    int result = 0;

    sigset_t mask;
    struct signalfd_siginfo sig_info;

    sigemptyset(&mask);

    for(int i = 0; i < NUMBER_OF_SIGNALS; i++)
        sigaddset(&mask, __signals__[i]);

    // Block signals to be handled by default handler
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
    {
        perror("sigprocmask");
        result = -1;
    }

    int sig_fd = signalfd(-1, &mask, 0);

    // SIGKILL and SIGSTOP we can not avoid so it is
    // not nessesary to OR with them too
    while(sig_info.ssi_signo != SIGTERM && sig_info.ssi_signo != SIGQUIT)
    {
        if(read(sig_fd, &sig_info, sizeof(struct signalfd_siginfo)) < 0)
        {
            perror("read");
            result = -1;
        }

        printf("Signal %d (%s) came from process with PID=%d (UID=%d) and message number: %d\n", sig_info.ssi_signo,  strsignal((int)sig_info.ssi_signo), 
                                                                                                 sig_info.ssi_pid,    sig_info.ssi_uid, sig_info.ssi_int);
    }

    if(close(sig_fd) < 0)
    {
        perror("close");
        result = -1;
    }

    return result;
}