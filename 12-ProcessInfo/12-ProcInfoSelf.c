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

#define FLAG_SCHED 1        // 1 << 0
#define FLAG_USAGE 2        // 1 << 1
#define FLAG_LIMITS 4       // 1 << 2
#define FLAG_NMSPC 8        // 1 << 3
#define FLAG_CPBTS 16       // 1 << 4
#define FLAG_EXTEND 32      // 1 << 5
#define FLAG_ENVIR 64       // 1 << 6

#define LIMITS_NUM 16       // Number of limits

//#define TEST_RR           //define this to test round robin policy for scheduler


void print_usage(struct rusage* rg);
ssize_t display_proc_info(pid_t proc_id, int flags);


int main(int argc, char* argv[])
{   
    int flags = 0;

    for(int i = 1; i < argc; i++)
    {
        if(strchr(argv[i], 's'))
            flags = flags | FLAG_SCHED;
        if(strchr(argv[i], 'u'))
            flags = flags | FLAG_USAGE;
        if(strchr(argv[i], 'l'))
            flags = flags | FLAG_LIMITS;
        if(strchr(argv[i], 'n'))
            flags = flags | FLAG_NMSPC;
        if(strchr(argv[i], 'c'))
            flags = flags | FLAG_CPBTS;
        if(strchr(argv[i], 'x'))
            flags = flags | FLAG_EXTEND;
        if(strchr(argv[i], 'e'))
            flags = flags | FLAG_ENVIR;
    }

    pid_t proc_id = getpid();

    if(display_proc_info(proc_id, flags) < 0)
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


ssize_t display_proc_info(pid_t proc_id, int flags)
{

    int result = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                     IDs                                                        //

    // NGROUPS_MAX defined in limits.h
    int max_size = NGROUPS_MAX;
    gid_t gid_list[max_size];

    printf("========================================================================================\n");
    printf("------------------------------------IDs and groups--------------------------------------\n");
    printf("PID: %d, PPID: %d, PGID: %d, SID: %d\n", proc_id, getppid(), getpgid(0), getsid(0));
    printf("TIDs: %d\n", gettid());

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

 
    if(flags & FLAG_SCHED)
    {
        printf("---------------------------------------SCHEDULER----------------------------------------\n");

        // obtaining scheduler policy
        int policy = sched_getscheduler(proc_id);       // SCHED_OTHER = 0, SCHED_FIFO = 1,  
                                                        // SCHED_RR = 2,    SCHED_BATCH = 3,  
                                                        // SCHED_IDLE = 5,  SCHED_DEADLINE = 6

#ifdef TEST_RR
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


    if(flags & FLAG_USAGE)
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
    if(flags & FLAG_LIMITS)
    {
        struct rlimit rm;
        printf("---------------------LIMITS---------------------------------SOFT-----HARD---------------\n");

        const int limits[LIMITS_NUM] = 
        {
            RLIMIT_AS,
            RLIMIT_CORE,
            RLIMIT_CPU,
            RLIMIT_DATA,
            RLIMIT_FSIZE,
            RLIMIT_LOCKS,
            RLIMIT_MEMLOCK,
            RLIMIT_MSGQUEUE,
            RLIMIT_NICE,
            RLIMIT_NOFILE,
            RLIMIT_NPROC,
            RLIMIT_RSS,
            RLIMIT_RTPRIO,
            RLIMIT_RTTIME,
            RLIMIT_SIGPENDING,
            RLIMIT_STACK
        };

        const char* limits_disp[LIMITS_NUM] = 
        {  
            "Process's virtual memory maximum size:       ",
            "Maximum size of a core file:                 ",
            "CPU time consumption in seconds:             ",
            "Maximum size of process's data segment:      ",
            "Maximum size of files process may create:    ",
            "Maximum number of locks:                     ",
            "Maximum memory may be locked into RAM:       ",
            "Maximum bytes allocated for queue messages:  ",
            "Maximum nice value:                          ",
            "Maximum file descriptor number:              ",
            "Maximum number of extant process:            ",
            "Maximum process's resident set in bytes:     ",
            "Maximum real-time priority:                  ",
            "Scheduled CPU time (microseconds):           ",
            "Number of signals may be queued:             ",
            "Maximum size of the process stack in bytes:  "
        };


        char limit_buf[sizeof("Process's virtual memory maximum size:       01234567890123456789 / 01234567890123456789\n")];
        char* offset = NULL;
        
        for(int i = 0; i < LIMITS_NUM; i++)
        {
            offset = limit_buf;
            if(getrlimit(limits[i], &rm) < 0)
            {
                perror("getrlimit");
                result = -1;
            }

            offset += sprintf(offset, "%s" , limits_disp[i]);
            if(rm.rlim_cur == RLIM_INFINITY) 
                offset += sprintf(offset, "%20s / " , "Unlimited");
            else
                offset += sprintf(offset, "%20lu / ", rm.rlim_cur);

            if(rm.rlim_max == RLIM_INFINITY) 
                offset += sprintf(offset, "%-20s" , "Unlimited");
            else
                offset += sprintf(offset, "%-20lu", rm.rlim_cur);
            
            printf("%s\n", limit_buf);
        }

        printf("========================================================================================\n");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                               NAMESPACES INFO                                                  //


    if(flags & FLAG_NMSPC)
    {
        printf("---------------------------------------NAMESPACES---------------------------------------\n");


        char buf[sizeof("cgroup:[0123456789]")];                        // longest namespace inode info string
        char ns_str[sizeof("/proc//ns/cgroup") + MAX_PID_LENGHT];       // max size of string
        memset(buf, 0, sizeof(buf));

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


    if(flags & FLAG_CPBTS)
    {
        printf("---CAPABILITIES----------EFFECTIVE----PERMITTED----INHERITABLE----BOUNDING----AMBIENT---\n");


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
            printf("%-28s", cap_name[i]);

            cap_get_flag(caps, cap_list[i], CAP_EFFECTIVE, &cap_flags_value);
            printf("%-12s ", (cap_flags_value == CAP_SET) ? "Yes" : "No");

            cap_get_flag(caps, cap_list[i], CAP_PERMITTED, &cap_flags_value);
            printf("%-13s ", (cap_flags_value == CAP_SET) ? "Yes" : "No");

            cap_get_flag(caps, cap_list[i], CAP_INHERITABLE, &cap_flags_value);
            printf("%-13s ", (cap_flags_value == CAP_SET) ? "Yes" : "No");

            printf("%-11s ", ((cap_get_bound(cap_list[i]))   && cap_get_bound(cap_list[i])   != -1) ? "Yes" : "No");

            printf("%-10s ", ((cap_get_ambient(cap_list[i])) && cap_get_ambient(cap_list[i]) != -1) ? "Yes" : "No");
            printf("\n");
        }

        cap_free(caps);
        printf("========================================================================================\n");
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                                EXTENDED INFO                                                   //

    if(flags & (FLAG_EXTEND | FLAG_ENVIR))
    {
        // from readproc.h
        proc_t proc_info;
        memset(&proc_info, 0, sizeof(proc_info));
        PROCTAB *pt_ptr = openproc(PROC_FILLSTATUS | PROC_PID     | PROC_FILLCOM | PROC_FILLENV |
                                   PROC_FILLUSR    | PROC_FILLGRP, &proc_id);

        readproc(pt_ptr, &proc_info);

        closeproc(pt_ptr);

        if(flags & FLAG_EXTEND)
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

        if(flags & FLAG_ENVIR)
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