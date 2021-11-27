#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <mqueue.h>

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        printf("Usage: %s /queuename\n", argv[0]);
        return 1;
    }

    // create a new queue or open existing one 
    mqd_t queue = mq_open(argv[1], O_RDWR | O_CREAT, 0600, NULL);

    if(queue == (mqd_t) -1)
    {
        perror("mq_open");
        return 1;
    }

    // send some test messages
    if(mq_send(queue, "Hello!", strlen("Hello!"), 0) == -1)
        perror("mq_send");

    if(mq_send(queue, "Second hello!", strlen("Second hello!"), 0) == -1)
        perror("mq_send");

    // get and show queue info
    struct mq_attr queue_info = {};
    mq_getattr(queue, &queue_info);

    printf("Flags:                          %ld\n", queue_info.mq_flags);
    printf("Max number of messages:         %ld\n", queue_info.mq_maxmsg);
    printf("Max size of message:            %ld\n", queue_info.mq_msgsize);
    printf("Number of messages in queue:    %ld\n", queue_info.mq_curmsgs);

    // preparing to read messages
    char message[queue_info.mq_msgsize];
    memset(message, 0, sizeof(message));

    // reading messages
    printf("Messages:\n");
    int number_of_messages = (int)queue_info.mq_curmsgs;
    for(int i = 0; i < number_of_messages; i++)
    {
        mq_receive(queue, message, sizeof(message), NULL);
        printf("'%s'\n", message);
    }


    // clean up
    mq_unlink(argv[1]);
    mq_close(queue);

    return 0;
}