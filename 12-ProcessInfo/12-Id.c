#define _GNU_SOURCE
#include <sys/capability.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <poll.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/resource.h>
#include <proc/readproc.h>

#define MAX_PID 4194304     // Maximum PID 2^22
#define MAX_PID_LENGHT 7    // Maximum PID lenght

//#define _TEST_RR          //define this to test round robin policy for scheduler

void print_usage(struct rusage* rg);
ssize_t display_proc_info(pid_t proc_id, unsigned scheduler_bool, unsigned usage_bool, unsigned limits_bool,    
                                         unsigned namespace_bool, unsigned capabilities_bool, unsigned extend_bool,
                                         unsigned envir_bool);


int main(int argc, char* argv[])
{   
    unsigned sched_bool = 0,
             us_bool = 0,
             lim_bool = 0,
             ns_bool = 0,
             cap_bool = 0,
             ext_bool = 0,
             env_bool = 0;

    for(int i = 1; i < argc; i++)
    {
        if(strchr(argv[i], 's'))
            sched_bool = 1;
        if(strchr(argv[i], 'u'))
            us_bool = 1;
        if(strchr(argv[i], 'l'))
            lim_bool = 1;
        if(strchr(argv[i], 'n'))
            ns_bool = 1;
        if(strchr(argv[i], 'c'))
            cap_bool = 1;
        if(strchr(argv[i], 'x'))
            ext_bool = 1;
        if(strchr(argv[i], 'e'))
            env_bool = 1;
    }

    pid_t proc_id = getpid();

    // Might be possible implemented near future
/*  if(us_bool)
    {   
        char buf;
        int poll_num = 0;
        struct pollfd fd;

        fd.fd = STDIN_FILENO;
        fd.events = POLLIN;

        while(1)
        {

            if(system("clear") < 0)
            {
                perror("system");
                return -1;
            }
            if(display_proc_info(proc_id ,sched_bool, us_bool, lim_bool, ns_bool, cap_bool, ext_bool) < 0)
            {
                perror("display_proc_info");
                return -1;
            }


            printf("Press ENTER to terminate\n");

            usleep(10000);

            poll_num = poll(&fd, 1, 100);


            if (poll_num == -1) 
            {
                if (errno == EINTR)
                    continue;
                perror("poll");
                return -1;
            }
            
            if(poll_num > 0)
            {
                if (fd.revents & POLLIN)
                {
                    if(read(STDIN_FILENO, &buf, 1) > 0 && buf == '\n')
                    {
                        break;
                    }
                }
            }

        }
    }
    else */
    if(display_proc_info(proc_id, sched_bool, us_bool, lim_bool, ns_bool, cap_bool, ext_bool, env_bool) < 0)
            {
                perror("display_proc_info");
                return -1;
            }


    return 0;
}


// function to print process's usage
void print_usage(struct rusage* rg)
{
    printf("Seconds spent executing in user mode:                             %ld.%06ld\n", rg->ru_utime.tv_sec, rg->ru_utime.tv_usec);
    printf("Seconds spent executing in kernel mode:                           %ld.%06ld\n", rg->ru_stime.tv_sec, rg->ru_stime.tv_usec);
    printf("Maximum resident set size:                                        %ld\n", rg->ru_maxrss);
    printf("Integral shared memory size:                                      %ld\n", rg->ru_ixrss);
    printf("Integral unshared data size:                                      %ld\n", rg->ru_idrss);
    printf("Integral unshared stack size:                                     %ld\n", rg->ru_isrss);
    printf("Page reclaims (soft page faults):                                 %ld\n", rg->ru_minflt);
    printf("Page faults (hard page faults):                                   %ld\n", rg->ru_majflt);
    printf("Swaps:                                                            %ld\n", rg->ru_nswap);
    printf("Block input operations:                                           %ld\n", rg->ru_inblock);
    printf("Block output operations:                                          %ld\n", rg->ru_oublock);
    printf("IPC messages sent:                                                %ld\n", rg->ru_msgsnd);
    printf("IPC messages received:                                            %ld\n", rg->ru_msgrcv);
    printf("Signals received:                                                 %ld\n", rg->ru_nsignals);
    printf("Voluntary context switches:                                       %ld\n", rg->ru_nvcsw);
    printf("Involuntary context switches:                                     %ld\n", rg->ru_nivcsw);
}


ssize_t display_proc_info(pid_t proc_id, unsigned scheduler_bool, unsigned usage_bool, unsigned limits_bool,    
                                         unsigned namespace_bool, unsigned capabilities_bool, unsigned extend_bool,
                                         unsigned envir_bool)
{

    int result = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                     IDs                                                        //

    // NGROUPS_MAX defined in limits.h
    int max_size = NGROUPS_MAX;
    gid_t gid_list[max_size];

    printf("========================================================================================\n");
    printf("------------------------------------IDs and groups--------------------------------------\n");
    printf("PID: %d, PPID: %d, PGID: %d, SID: %d, TID: %d\n", proc_id, getppid(), getpgid(0), getsid(0), gettid());

    printf("UID: %d, GID: %d\n", getuid(), getgid());
    printf("Effective UID: %d\n", geteuid());
    printf("Effective GID: %d\n", getegid());

    // number of process's groups
    int groups_num = getgroups(max_size, gid_list);
    if(groups_num < 0)
    {
        perror("getgroups");
        result = -1;
    }

    printf("Groups: ");
    for(int i = 0; i < groups_num - 1; i++)
        printf("%d, ", gid_list[i]);

    printf("%d\n", gid_list[groups_num - 1]);

    printf("========================================================================================\n");



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                SCHEDULER INFO                                                  //

 
    if(scheduler_bool)
    {
        printf("---------------------------------------SCHEDULER----------------------------------------\n");

        // obtaining scheduler policy
        int policy = sched_getscheduler(proc_id);       // SCHED_OTHER = 0, SCHED_FIFO = 1,  
                                                        // SCHED_RR = 2,    SCHED_BATCH = 3,  
                                                        // SCHED_IDLE = 5,  SCHED_DEADLINE = 6

        #ifdef _TEST_RR
        policy = SCHED_RR;
        #endif

        printf("Policy:                                                           %d\n", policy);
        printf("Priority:                                                         %d\n", getpriority(PRIO_PROCESS, (unsigned int)proc_id));
        
        // for those policies priority always 0
        if(policy != SCHED_OTHER && policy != SCHED_IDLE && policy != SCHED_BATCH)
        {
            printf("Maximum priority value that can be used by scheduler:             %d\n", sched_get_priority_max(policy));
            printf("Minumum priority value that can be used by scheduler:             %d\n", sched_get_priority_min(policy));
        }

        // if policy is round robin, then display time quantum
        if(policy == SCHED_RR)
        {
            struct timespec quantum;
            if(sched_rr_get_interval(proc_id, &quantum) < 0)
            {
                perror("sched_rr_get_interval");
                result = -1;
            }

            printf("Round robin quantum in seconds:                                   %ld.%09ld\n", quantum.tv_sec, quantum.tv_nsec);

        }

        // CPUs
        printf("CPU number                                                        %u\n", sched_getcpu());

        cpu_set_t mask;
    /*  struct cpu_set_t 
        {
            __bits[__CPU_SETSIZE / __NCPUBITS];            just to remember
        };
    */

        if(sched_getaffinity(proc_id, sizeof(cpu_set_t), &mask) < 0)
        {
            perror("sched_getaffinity");
            result = -1;
        }
        
        int CPUs = CPU_COUNT(&mask);
        printf("Number of CPUs:                                                   %d\n", CPUs);

        printf("CPUs: ");
        size_t i = 0;
        for(i = 0; (int)i < CPUs; i++)
            if(CPU_ISSET(i, &mask))
                printf("%lu ", i + 1);

        printf("\n");
        printf("========================================================================================\n");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                                    USAGE                                                       //


    if(usage_bool)
    {
        struct rusage rg;

        if(getrusage(RUSAGE_SELF, &rg) < 0)
        {
            perror("getrusage");
            result = -1;
        }

        printf("---------------------------------------SELF USAGE---------------------------------------\n");
        print_usage(&rg);

        if(getrusage(RUSAGE_CHILDREN, &rg) < 0)
        {
            perror("getrusage");
            result = -1;
        }

        printf("-------------------------------------CHILDREN USAGE-------------------------------------\n");
        print_usage(&rg);

        if(getrusage(RUSAGE_THREAD, &rg) < 0)
        {
            perror("getrusage");
            result = -1;
        }

        printf("--------------------------------------THREAD USAGE--------------------------------------\n");
        print_usage(&rg);
        printf("========================================================================================\n");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                                 LIMITS INFO                                                    //
    if(limits_bool)
    {
        struct rlimit rm;
        printf("---------------------LIMITS--------------------------SOFT-------------------HARD--------\n");

        // 1)
        if(getrlimit(RLIMIT_AS, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("Process's virtual memory maximum size:       %20lu / %20lu\n", rm.rlim_cur, rm.rlim_max);

        // 2)
        if(getrlimit(RLIMIT_CORE, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("Maximum size of a core file:                 %20lu / %20lu\n", rm.rlim_cur, rm.rlim_max);

        // 3)
        if(getrlimit(RLIMIT_CPU, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("CPU time consumption in seconds:             %20lu / %20lu\n", rm.rlim_cur, rm.rlim_max);

        // 4)
        if(getrlimit(RLIMIT_DATA, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("Maximum size of process's data segment:      %20lu / %20lu\n", rm.rlim_cur, rm.rlim_max);

        // 5)
        if(getrlimit(RLIMIT_FSIZE, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("Maximum size of files process may create:    %20lu / %20lu\n", rm.rlim_cur, rm.rlim_max);

        // 6)
        if(getrlimit(RLIMIT_LOCKS, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("Maximum number of locks:                     %20lu / %20lu\n", rm.rlim_cur, rm.rlim_max);

        // 7)
        if(getrlimit(RLIMIT_MEMLOCK, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("Maximum memory may be locked into RAM:       %20lu / %20lu\n", rm.rlim_cur, rm.rlim_max);

        // 8)
        if(getrlimit(RLIMIT_MSGQUEUE, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("Maximum bytes allocated for queue messages:  %20lu / %20lu\n", rm.rlim_cur, rm.rlim_max);

        // 9)
        if(getrlimit(RLIMIT_NICE, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("Maximum nice value:                          %20lu / %20lu\n", rm.rlim_cur, rm.rlim_max);

        // 10)
        if(getrlimit(RLIMIT_NOFILE, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("Maximum file descriptor number:              %20lu / %20lu\n", rm.rlim_cur - 1, rm.rlim_max - 1);

        // 11)
        if(getrlimit(RLIMIT_NPROC, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("Maximum number of extant process:            %20lu / %20lu\n", rm.rlim_cur, rm.rlim_max);

        // 12)
        if(getrlimit(RLIMIT_RSS, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("Maximum process's resident set in bytes:     %20lu / %20lu\n", rm.rlim_cur, rm.rlim_max);

        // 13)
        if(getrlimit(RLIMIT_RTPRIO, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("Maximum real-time priority:                  %20lu / %20lu\n", rm.rlim_cur, rm.rlim_max);

        // 14)
        if(getrlimit(RLIMIT_RTTIME, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("Scheduled CPU time (microseconds):           %20lu / %20lu\n", rm.rlim_cur, rm.rlim_max);

        // 15)
        if(getrlimit(RLIMIT_SIGPENDING, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("Number of signals may be queued:             %20lu / %20lu\n", rm.rlim_cur, rm.rlim_max);

        // 16)
        if(getrlimit(RLIMIT_STACK, &rm) < 0)
        {
            perror("getrlimit");
            result = -1;
        }
        printf("Maximum size of the process stack in bytes:  %20lu / %20lu\n", rm.rlim_cur, rm.rlim_max);

        
        printf("========================================================================================\n");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                               NAMESPACES INFO                                                  //


    if(namespace_bool)
    {
        printf("---------------------------------------NAMESPACES---------------------------------------\n");


        char buf[sizeof("cgroup:[0123456789]")];                        // longest namespace inode info string
        char ns_str[sizeof("/proc//ns/cgroup") + MAX_PID_LENGHT];       // max size of string

        // pid namespace
        sprintf(ns_str, "/proc/%d/ns/pid", proc_id);

        if(readlink(ns_str, buf, sizeof(buf)) < 0)
        {
            perror("readlink");
            result = -1;
        }
        printf("%s\n", buf);

        // mnt namespace
        sprintf(ns_str, "/proc/%d/ns/mnt", proc_id);

        if(readlink(ns_str, buf, sizeof(buf)) < 0)
        {
            perror("readlink");
            result = -1;
        }
        printf("%s\n", buf);

        // net namespace
        sprintf(ns_str, "/proc/%d/ns/net", proc_id);

        if(readlink(ns_str, buf, sizeof(buf)) < 0)
        {
            perror("readlink");
            result = -1;
        }
        printf("%s\n", buf);

        // ipc namespace
        sprintf(ns_str, "/proc/%d/ns/ipc", proc_id);

        if(readlink(ns_str, buf, sizeof(buf)) < 0)
        {
            perror("readlink");
            result = -1;
        }
        printf("%s\n", buf);

        // uts namespace
        sprintf(ns_str, "/proc/%d/ns/uts", proc_id);

        if(readlink(ns_str, buf, sizeof(buf)) < 0)
        {
            perror("readlink");
            result = -1;
        }
        printf("%s\n", buf);

        // user namespace
        sprintf(ns_str, "/proc/%d/ns/user", proc_id);

        if(readlink(ns_str, buf, sizeof(buf)) < 0)
        {
            perror("readlink");
            result = -1;
        }
        printf("%s\n", buf);

        // cgroup namespace
        sprintf(ns_str, "/proc/%d/ns/cgroup", proc_id);

        if(readlink(ns_str, buf, sizeof(buf)) < 0)
        {
            perror("readlink");
            result = -1;
        }
        printf("%s\n", buf);

        printf("========================================================================================\n");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                              CAPABILITIES INFO                                                 //


    if(capabilities_bool)
    {
        printf("----CAPABILITIES----------------------------EFFECTIVE----PERMITTED----INHERITABLE-------\n");


        cap_t caps;
        cap_value_t cap_list[CAP_LAST_CAP + 1];
        cap_flag_value_t cap_flags_value;

        const char *cap_name[CAP_LAST_CAP + 1] = 
        {
            "CAP_CHOWN",
            "CAP_DAC_OVERRIDE",
            "CAP_DAC_READ_SEARCH",
            "CAP_FOWNER",
            "CAP_FSETID",
            "CAP_KILL",
            "CAP_SETGID",
            "CAP_SETUID",
            "CAP_SETPCAP",
            "CAP_LINUX_IMMUTABLE",
            "CAP_NET_BIND_SERVICE",
            "CAP_NET_BROADCAST",
            "CAP_NET_ADMIN",
            "CAP_NET_RAW",
            "CAP_IPC_LOCK",
            "CAP_IPC_OWNER",
            "CAP_SYS_MODULE",
            "CAP_SYS_RAWIO",
            "CAP_SYS_CHROOT",
            "CAP_SYS_PTRACE",
            "CAP_SYS_PACCT",
            "CAP_SYS_ADMIN",
            "CAP_SYS_BOOT",
            "CAP_SYS_NICE",
            "CAP_SYS_RESOURCE",
            "CAP_SYS_TIME",
            "CAP_SYS_TTY_CONFIG",
            "CAP_MKNOD",
            "CAP_LEASE",
            "CAP_AUDIT_WRITE",
            "CAP_AUDIT_CONTROL",
            "CAP_SETFCAP",
            "CAP_MAC_OVERRIDE",
            "CAP_MAC_ADMIN",
            "CAP_SYSLOG",
            "CAP_WAKE_ALARM",
            "CAP_BLOCK_SUSPEND",
            "CAP_AUDIT_READ"
        };

        caps = cap_get_pid(proc_id);
        if (caps == NULL) {
            perror("cap_get_pid");
            result = -1;
        }


        for (int i = 0; i < CAP_LAST_CAP + 1; i++) 
        {
            cap_from_name(cap_name[i], &cap_list[i]);
            printf("%-33s\t\t", cap_name[i]);

            cap_get_flag(caps, cap_list[i], CAP_EFFECTIVE, &cap_flags_value);
            printf("%-12s ", (cap_flags_value == CAP_SET) ? "Yes" : "No");
            cap_get_flag(caps, cap_list[i], CAP_PERMITTED, &cap_flags_value);
            printf("%-13s ", (cap_flags_value == CAP_SET) ? "Yes" : "No");
            cap_get_flag(caps, cap_list[i], CAP_INHERITABLE, &cap_flags_value);
            printf("%-13s ", (cap_flags_value == CAP_SET) ? "Yes" : "No");
            printf("\n");
        }

        cap_free(caps);
        printf("========================================================================================\n");
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                                EXTENDED INFO                                                   //

    if(extend_bool || envir_bool)
    {
        // from readproc.h
        proc_t proc_info;
        memset(&proc_info, 0, sizeof(proc_info));
        PROCTAB *pt_ptr = openproc(PROC_FILLSTATUS | PROC_PID     | PROC_FILLCOM | PROC_FILLENV |
                                   PROC_FILLUSR    | PROC_FILLGRP, &proc_id);

        readproc(pt_ptr, &proc_info);

        closeproc(pt_ptr);

        if(extend_bool)
        {
            printf("----------------------------------------EXTENDED----------------------------------------\n");
            
            printf("Process's name:                                                   %s\n", *proc_info.cmdline);
            printf("Process's state:                                                  %c\n",  proc_info.state);
            printf("Nice level:                                                       %ld\n", proc_info.nice);
            printf("Total number of pages:                                            %ld\n", proc_info.size);
            printf("Dirty pages:                                                      %ld\n", proc_info.dt);
            printf("Number of pages in virtual memory:                                %lu\n", proc_info.vsize);
            printf("Size of pages in virtual memory:                                  %lu\n", proc_info.vm_size);
            printf("Locked pages in Kbytes:                                           %lu\n", proc_info.vm_lock);
            printf("Data size:                                                        %lu\n", proc_info.vm_data);
            printf("Stack size:                                                       %lu\n", proc_info.vm_stack);
            printf("Executable size:                                                  %lu\n", proc_info.vm_exe);
            printf("Library size:                                                     %lu\n", proc_info.vm_lib);
            printf("Real user name:                                                   %s (%d)\n", proc_info.ruser, proc_info.ruid);
            printf("Effective user name:                                              %s (%d)\n", proc_info.euser, proc_info.euid);
            printf("Saved user name:                                                  %s (%d)\n", proc_info.suser, proc_info.suid);
            printf("Real group name:                                                  %s (%d)\n", proc_info.rgroup, proc_info.rgid);
            printf("Effective group name:                                             %s (%d)\n", proc_info.egroup, proc_info.egid);
            printf("Saved group name:                                                 %s (%d)\n", proc_info.sgroup, proc_info.sgid);
            printf("Filesystem group name:                                            %s\n", proc_info.fgroup);
            printf("Basename of executable file:                                      %s\n", proc_info.cmd);
            printf("Full device number of controlling terminal (TTY):                 %d\n", proc_info.tty);
            printf("Terminale process group id:                                       %d\n", proc_info.tpgid);
            printf("========================================================================================\n");
        }

        if(envir_bool)
        {
            printf("--------------------------------------ENVIRONMENT---------------------------------------\n");

            int i = 0;
            while(proc_info.environ[i])
            {
                printf("%s\n", proc_info.environ[i]);
                i++;
            }

        }

    }


    return result;

}