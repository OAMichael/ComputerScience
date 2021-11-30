#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <math.h>
#include <pthread.h>
#include <errno.h>


#define MAX_SETTINGS_LENGHT 65535   // 2 << 16 - 1
//#define TEST_INTEGRAL             // Uncomment this line to test only integral


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
    pthread_mutex_t* m;
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

    pthread_mutex_lock(w->m);
    *(integral->p_total_sum) += local_sum;
    pthread_mutex_unlock(w->m);

    return NULL;
}


int main(int argc, char* argv[])
{
    if (argc < 5)
    {
        printf("Usage: %s <'function(x)'> <start> <end> <#points> [#threads]\n", argv[0]);
        return 1;
    }

    int number_of_threads = 4;
    if(argc > 5)
        number_of_threads = atoi(argv[5]);

    int pipe_fds[2] = {};

    if(pipe(pipe_fds) < 0)
    {
        perror("pipe");
        return 2;
    }


    pid_t child_id = fork();
    if(child_id < 0)
    { 
        perror("fork_1");
        return 3;
    }

    if(child_id == 0)
    {
        // This code executed in child process only
        close(pipe_fds[1]);

        if(dup2(pipe_fds[0], fileno(stdin)) < 0)
        {
            perror("dup2");
            return 4;
        }
        close(pipe_fds[0]);

        // Start compilation
        execlp(
                // executable
                "gcc", 
                // argv[0], argv[1], ...
                "gcc", "-Wall", "-Wextra", 
                // produce position-independent dynamic library (shared option)
                "-fPIC", "-fPIE", "-shared",
                // enable optimisations
                "-O2", "-fomit-frame-pointer", "-march=native", "-mtune=native",
                // read input from stdin, treat it as a C source
                "-xc", "-",
                // link with linkm
                "-lm", 
                // save output to tmp.so
                "-o", "tmp.so", NULL);

        perror("exec");
        return 5;
    }
    else
    {
        // This code executed in parent process only
        dprintf(pipe_fds[1],
                "#include <math.h>\n"
                "double tmpfun(double x) { return %s; }\n", argv[1]);
        
        close(pipe_fds[1]);


        // Wait for compilation end and check its status
        int status;
        waitpid(child_id, &status, 0);

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) 
        {
            printf("Compilation error, gcc exit code %d\n", WEXITSTATUS(status));
            return 6;
        }

        // Try to load compiled library
        void* tmplib = dlopen("./tmp.so", RTLD_LAZY);
        char* dlerrstr;
        if (!tmplib) {
            perror(dlerror());
            return -1;
        }

        double(*tmpfun)(double);
        *(void**)(&tmpfun) = dlsym(tmplib, "tmpfun");
        if ((dlerrstr = dlerror()) != NULL) {
            puts(dlerrstr);
            return -1;
        }

        printf("Calculating the integral...\n");

        double total_sum = 0;

//=============================WHOLE THIS BLOCK MADE TO CORRECTLY SPREAD RESOURCES AMONG THE THREADS=============================

        // Initialize mutex
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_t tids[number_of_threads];

        double start = atof(argv[2]);
        double end   = atof(argv[3]);
        unsigned long points = strtoul(argv[4], NULL, 10);

        double per_thread_interval = (end - start) / (double)number_of_threads;

        integrate_info integr[number_of_threads];

        // Do such a bad stuff because it is possible that (points % number_of_threads != 0)
        unsigned long points_ar[number_of_threads];
        points_ar[number_of_threads - 1] = points;

        for(int i = 0; i < number_of_threads - 1; i++)
        {
            points_ar[i] = points / (unsigned long)number_of_threads;
            points_ar[number_of_threads - 1] -= points_ar[i];
        }


        // Resources and integration information for the main thread
        integr[number_of_threads - 1].function = tmpfun;
        integr[number_of_threads - 1].start    = end - per_thread_interval;
        integr[number_of_threads - 1].end      = end;
        integr[number_of_threads - 1].points   = points_ar[number_of_threads - 1];
        integr[number_of_threads - 1].p_total_sum = &total_sum;

        work_area_t work_area_main = {
            .m = &mutex,
            .i_ptr = &integr[number_of_threads - 1]
        };


        // Spreading integration information among secondary threads
        work_area_t work_area[number_of_threads - 1];
        for(int i = 0; i < number_of_threads - 1; i++)
        {
            integr[i].function = tmpfun;
            integr[i].start    = start + i * per_thread_interval;
            integr[i].end      = start + (i + 1) * per_thread_interval;
            integr[i].points   = points_ar[i];
            integr[i].p_total_sum = &total_sum;

            work_area[i].m = &mutex;
            work_area[i].i_ptr = &integr[i];
        }

        for(int i = 0; i < number_of_threads - 1; i++)
            if((errno = pthread_create(&tids[i], NULL, integrate, &work_area[i])))
            {
                perror("pthread_create");
                return -1;
            }

        integrate(&work_area_main);

//===============================================================================================================================

        for(int i = 0; i < number_of_threads - 1; i++)
            pthread_join(tids[i], NULL);


        printf("Integral of %s (from x=%s to x=%s) = %lf\n", argv[1], argv[2], argv[3], total_sum);

        dlclose(tmplib);

#ifdef TEST_INTEGRAL
    
    return 0;

#endif
        printf("Plotting...\n");

        // gnuplot call
        char settings[MAX_SETTINGS_LENGHT] = {}; 
        
        snprintf(settings, sizeof(settings),   "set grid;\
                              set terminal wxt size 1200, 900;\
                              set samples %lu;\
                              set xlabel 'X';\
                              set ylabel 'Y';\
                              set xrange [%s:%s];\
                              set zeroaxis linetype 2;\
                              set title '%s';\
                              plot %s", (strtoul(argv[4], NULL, 10) < 1000000) ? strtoul(argv[4], NULL, 10) : 1000000, argv[2], argv[3], argv[1], argv[1]);
        
        execlp("gnuplot", 
               "gnuplot", "-p", "-e", 
                settings, NULL);

        perror("execlp");
    }

    return 0;
}