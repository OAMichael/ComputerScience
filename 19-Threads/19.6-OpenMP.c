#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define NUMBER_OF_THREADS 4

typedef struct 
{
    double (*function)(double);
    
    double start,
           end;

    unsigned long points;
    double* p_total_sum;

} integrate_info;


void integrate(integrate_info* integral)
{   
    // Calculating integration parameters for THIS thread
    int thread_num = omp_get_thread_num();

    double start = integral->start + thread_num * (integral->end - integral->start) / NUMBER_OF_THREADS;
    double end   = integral->start + (thread_num + 1) * (integral->end - integral->start) / NUMBER_OF_THREADS;
    unsigned long points = integral->points / NUMBER_OF_THREADS;

    if(end - start < 1e-8)
        return;

    double local_sum = 0;
    double dx = (end - start) / (double)points;

    unsigned long i = 0;
    while(start + (double)i * dx < end)
    {
        local_sum += (integral->function(start + (double)i * dx) + 
                      integral->function(start + (double)(i + 1) * dx)) / 2.0;

        i++;
    }

    local_sum *= dx;

    // Adding local_sum to global one 
    #pragma omp atomic
    *(integral->p_total_sum) += local_sum;

    return;
}


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("Usage: %s <start> <end> <#points>\n", argv[0]);
        return -1;
    }

    double total_sum = 0;

    integrate_info integr = {
        .function = sin,
        .start = atof(argv[1]),
        .end   = atof(argv[2]),
        .points = strtoul(argv[3], NULL, 10),
        .p_total_sum = &total_sum
    };

    // prohibit OpenMP to change number of threads
    omp_set_dynamic(0);
    // set number of threads
    omp_set_num_threads(NUMBER_OF_THREADS);

    // Integrating...
    #pragma omp parallel shared(integr)
    integrate(&integr);    

    printf("%lf\n", total_sum);

    return 0;
}