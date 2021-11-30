#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <math.h>


#define MAX_SETTINGS_LENGHT 65535   // 2 << 16 - 1
//#define TEST_INTEGRAL             // Uncomment this line to test only integral


typedef struct {
    double (*function)(double);
    
    double start,
           end;

    unsigned long points;
    double* p_total_sum;

} integrate_info;


// Function which integrates mathematical function
// using trapezoidal rule
ssize_t integrate(integrate_info* integral)
{
    if(integral->end == integral->start)
        return 0;

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
        return -1;
    }

    local_sum *= dx;


    *(integral->p_total_sum) += local_sum;

    return 0;
}


int main(int argc, char* argv[])
{
    if (argc < 5)
    {
        printf("Usage: %s <'function(x)'> <start> <end> <#points>\n", argv[0]);
        return 1;
    }

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

        double total_sum = 0;

        integrate_info integr = {
            .function = tmpfun,
            .start    = atof(argv[2]),
            .end      = atof(argv[3]),
            .points   = strtoul(argv[4], NULL, 10),
            .p_total_sum = &total_sum
        };

        integrate(&integr);

        printf("Integral of %s (from x=%s to x=%s) = %lf\n", argv[1], argv[2], argv[3], total_sum);

        dlclose(tmplib);

#ifdef TEST_INTEGRAL
    
    return 0;

#endif
        
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
                              plot %s", integr.points, argv[2], argv[3], argv[1], argv[1]);
        
        execlp("gnuplot", 
               "gnuplot", "-p", "-e", 
                settings, NULL);

        perror("execlp");
    }

    return 0;
}