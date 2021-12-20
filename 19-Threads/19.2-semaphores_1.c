#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <math.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>


#define NUMBER_OF_FUNCTIONS 13


typedef struct 
{
    double (*function)(double);
    
    double start,
           end;

    unsigned long points;
    double* p_total_sum;

} integrate_info;


typedef struct 
{
    sem_t* sem;
    integrate_info* i_ptr;

} work_area_t;


// Function which integrates mathematical function
// using trapezoidal rule
void* integrate(void* arg)
{   
    work_area_t* w = (work_area_t*)arg;
    integrate_info* integral = w->i_ptr;

    if(integral->end == integral->start)
        return NULL;

    double local_sum = 0;
    double dx = (integral->end - integral->start) / (double)integral->points;

    unsigned long i = 0;
    while(integral->start + (double)i * dx < integral->end)
    {
        local_sum += (integral->function(integral->start + (double)i * dx) + 
                      integral->function(integral->start + (double)(i + 1) * dx)) / 2.0;

        i++;
    }

    if(isnan(local_sum))
    {
        printf("The function is discontinuous at [%lf : %lf]\n", integral->start, integral->end);
        return arg;      // Just return some rubbish, but NULL
    }

    local_sum *= dx;

    sem_wait(w->sem);
    *(integral->p_total_sum) += local_sum;
    sem_post(w->sem);

    return NULL;
}

//===================================================DEFINE ALL FUNCTION MESS====================================================

double sine(double x)   { return sin(x); }
double cosine(double x) { return cos(x); }
double tg(double x)     { return tan(x); }
double arcsin(double x) { return asin(x);}
double arccos(double x) { return acos(x);}
double arctg(double x)  { return atan(x);}
double lin(double x)    { return x;      }
double sqr(double x)    { return x * x;  }
double cube(double x)   { return x*x*x;  }
double recip(double x)  { return 1 / x;  }
double recsqr(double x) { return 1/(x*x);}
double sqroot(double x) { return sqrt(x);}
double cbroot(double x) { return cbrt(x);}

const char* functions_str[NUMBER_OF_FUNCTIONS] = {
    "sin(x)",
    "cos(x)",
    "tan(x)",
    "arcsin(x)",
    "arccos(x)",
    "arctan(x)",
    "x",
    "x^2",
    "x^3",
    "1/x",
    "1/x^2",
    "sqrt(x)",
    "cbrt(x)"
};

double (*functions[NUMBER_OF_FUNCTIONS])(double) = {
    sine,
    cosine,
    tg,
    arcsin,
    arccos,
    arctg,
    lin,
    sqr,
    cube,
    recip,
    recsqr,
    sqroot,
    cbroot
};

//===============================================================================================================================


int main(int argc, char* argv[])
{
    if (argc < 5)
    {
        if(argc > 1 && !strcmp(argv[1], "-l"))
        {
            printf("List of available functions:\n");
            for(int i = 0; i < NUMBER_OF_FUNCTIONS; i++)
            {
                printf("%d.\t%s\n", i + 1, functions_str[i]);
            }
            return 1;
        }

        printf("Usage: %s <#function> <start> <end> <#points> [#threads]\n", argv[0]);
        printf("For a list of available functions: %s -l\n", argv[0]);
        return 2;
    }

    int number_of_threads = 4;
    if(argc > 5)
        number_of_threads = atoi(argv[5]);

    double (*tmpfun)(double) = functions[atoi(argv[1]) - 1];
       
    printf("Calculating the integral...\n");

    double total_sum = 0;

//=============================WHOLE THIS BLOCK MADE TO CORRECTLY SPREAD RESOURCES AMONG THE THREADS=============================

    // Initialize semaphore
    sem_t semaphore;
    sem_init(&semaphore, 0, 1);
    pthread_t tids[number_of_threads];

    double start = atof(argv[2]);
    double end   = atof(argv[3]);
    unsigned long points = strtoul(argv[4], NULL, 10);

    double per_thread_interval = (end - start) / (double)number_of_threads;

    integrate_info integr[number_of_threads];

    // Do such a bad stuff because it is possible that (points % number_of_threads) != 0
    unsigned long points_ar[number_of_threads];
    points_ar[number_of_threads - 1] = points;

    for(int i = 0; i < number_of_threads - 1; i++)
    {
        points_ar[i] = points / (unsigned long)number_of_threads;
        points_ar[number_of_threads - 1] -= points_ar[i];
    }

    // Spreading integration information among secondary threads
    work_area_t work_area[number_of_threads];
    for(int i = 0; i < number_of_threads; i++)
    {
        integr[i].function = tmpfun;
        integr[i].start    = start + i * per_thread_interval;
        integr[i].end      = start + (i + 1) * per_thread_interval;
        integr[i].points   = points_ar[i];
        integr[i].p_total_sum = &total_sum;

        work_area[i].sem = &semaphore;
        work_area[i].i_ptr = &integr[i];
    }

    for(int i = 0; i < number_of_threads; i++)
        if((errno = pthread_create(&tids[i], NULL, integrate, &work_area[i])))
        {
            perror("pthread_create");
            return -1;
        }

//===============================================================================================================================

    for(int i = 0; i < number_of_threads; i++)
        pthread_join(tids[i], NULL);

    sem_destroy(&semaphore);

    printf("Integral of %s (from x=%s to x=%s) = %lf\n", functions_str[atoi(argv[1]) - 1], argv[2], argv[3], total_sum);

    return 0;
}