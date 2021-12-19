#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <mqueue.h>

int main(int argc, char* argv[])
{
    int result = 0;

    if(argc < 3)
    {
        printf("Usage: %s /queuename <message1> <message2> ...\n", argv[0]);
        return 1;
    }

    // opening the queue
    mqd_t queue = mq_open(argv[1], O_WRONLY);

    if(queue == (mqd_t) -1)
    {
        perror("mq_open");
        return 1;
    }

    for(int i = 2; i < argc; i++)
        if(mq_send(queue, argv[i], strlen(argv[i]), 0) == -1)
        {
            perror("mq_send"); 
            result = -1;
        }

    return result;
}
