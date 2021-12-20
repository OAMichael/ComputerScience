#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdatomic.h>


typedef struct
{
    atomic_ullong counter;
    unsigned long max_count;
} work_area_t;

void* thr_body(void* arg)
{
    work_area_t* w = arg;
    for(unsigned long i = 0; i < w->max_count; i++)
    {
        atomic_fetch_add(&w->counter, 1);
    }

    return NULL;
}


int main(int argc, char* argv[])
{
    unsigned long count_lim = 1000000;
    if(argc > 1)
    {
        count_lim = strtoul(argv[1], NULL, 10);
    }

    pthread_t tid;

    work_area_t work_area = { .max_count = count_lim };
    atomic_init(&work_area.counter, 0);

    if((errno = pthread_create(&tid, NULL, thr_body, &work_area)))
    {
        perror("pthread_create");
        return 1;
    }

    for(unsigned long i = 0; i < count_lim; i++)
    {
        atomic_fetch_add(&work_area.counter, 1);
    }

    pthread_join(tid, NULL);
    printf("%llu\n", atomic_load(&work_area.counter));

    return 0;
}