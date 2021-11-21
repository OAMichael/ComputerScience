#define _GNU_SOURCE
#include <sys/capability.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <linux/taskstats.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <poll.h>
#include <grp.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <signal.h>
#include <sys/resource.h>
#include <proc/readproc.h>

#define MAX_PID 4194304     // Maximum PID 2^22
#define MAX_PID_LENGHT 7    // Maximum PID lenght
#define MAX_TID 46332
#define MAX_TID_LENGHT 5

#define FLAG_SCHED 1        // 1 << 0
#define FLAG_USAGE 2        // 1 << 1
#define FLAG_LIMITS 4       // 1 << 2
#define FLAG_NMSPC 8        // 1 << 3
#define FLAG_CPBTS 16       // 1 << 4
#define FLAG_EXTEND 32      // 1 << 5
#define FLAG_ENVIR 64       // 1 << 6

#define LIMITS_NUM 16       // Number of limits

#define SIGNAL_STRING       // Used to display signals as strings 


ssize_t display_proc_info(const pid_t proc_id, const int flags);


int main(int argc, char* argv[])
{   
    int flags = 0;

    pid_t proc_id = getpid();
    
    // The way how to find out either user entered [PID] or not
    if(argc > 1)
    {
        unsigned letters = 0,
                 numbers = 0;

        for(long unsigned i = 0; i < strlen(argv[1]); i++)
        {
            if(isdigit(argv[1][i]))
                numbers++;
            else
                letters++;
        }

        if(numbers > 0)
        {
            if(numbers != strlen(argv[1]))
            {
                printf("Usage: %s [PID] [flags]\n", argv[0]);
                printf("Please enter correct PID\n");
                return -1;
            }
            else
                proc_id = atoi(argv[1]);
        }

        if(proc_id == 0)
        {
            printf("This process is Scheduler which isn't usual process to obtain information\n");
            return -1;
        }

        // checking if process with PID = proc_id exists
        if(kill(proc_id, 0) == -1 && errno == ESRCH)
        {
            printf("Such process doesn't exist\n");
            return -1;
        }

    }

    // Checking if some flags were entered
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

    // Just display all information dependent on entered flags
    if(display_proc_info(proc_id, flags) < 0)
    {
        perror("display_proc_info");
        return -1;
    }


    return 0;
}

ssize_t display_proc_info(const pid_t proc_id, const int flags)
{
    // Final returning value of this function
    int result = 0;

    // Different states of a process
    const char* state_list[9] = 
    {
        "Uninterruptible sleep",
        "Idle",
        "Running",
        "Interruptible sleep",
        "Stopped by signal",
        "Stopped by debugger",
        "Paging",
        "Dead",
        "Zombie"
    };

    // from readproc.h
    proc_t proc_info;
    memset(&proc_info, 0, sizeof(proc_info));
    PROCTAB *pt_ptr = openproc(PROC_FILLSTATUS  | PROC_PID     | PROC_FILLCOM    | PROC_FILLENV  |
                               PROC_FILLUSR     | PROC_FILLGRP | PROC_FILLMEM    | PROC_FILLSTAT |
                               PROC_FILLCGROUP  | PROC_FILLOOM | PROC_FILLSUPGRP | PROC_FILLNS   |
                               PROC_FILLSYSTEMD, &proc_id);

    readproc(pt_ptr, &proc_info);
    closeproc(pt_ptr);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                     IDs                                                        //

    printf("========================================================================================\n");
    printf("------------------------------------IDs and groups--------------------------------------\n");
    
    if(proc_info.cmdline != NULL)
        printf("Process's name:               %s\n", *proc_info.cmdline);
            
    printf("Basename of executable file:  %s\n", proc_info.cmd);

    printf("Process's state:              %c ", proc_info.state);

    switch(proc_info.state)
    {
        case 'D': printf("(%s)\n", state_list[0]); break;
        case 'I': printf("(%s)\n", state_list[1]); break;
        case 'R': printf("(%s)\n", state_list[2]); break;
        case 'S': printf("(%s)\n", state_list[3]); break;
        case 'T': printf("(%s)\n", state_list[4]); break;
        case 't': printf("(%s)\n", state_list[5]); break;
        case 'W': printf("(%s)\n", state_list[6]); break;
        case 'X': printf("(%s)\n", state_list[7]); break;
        case 'Z': printf("(%s)\n", state_list[8]); break;
    }

    printf("PID: %d, PPID: %d\nPGID: %d, SID: %d\n", proc_id, proc_info.ppid, getpgid(proc_id), getsid(proc_id));


    // Obtaining thread IDs of particular process
    // Actually struct proc_t has member tid, but it is only one, main thread id
    if(proc_info.nlwp == 1)
        printf("TIDs: %d\n", proc_info.tid);
    else
    {
    // The way of obtaining TIDs below allows to find out all TIDs of process 
        char* tids_buf[MAX_TID];
        char tid_str[sizeof("/proc//task") + MAX_TID_LENGHT + 1];       // max size of string
        memset(tids_buf, 0, sizeof(tids_buf));

        sprintf(tid_str, "/proc/%d/task", proc_id);

        DIR* dir_fd = opendir(tid_str);

        if(!dir_fd)
        {
            perror("opendir");
            return 1;
        }

        // Counter of TIDs
        int cntr = 0;
        struct dirent* entry;
        while ((entry = readdir(dir_fd)) != NULL)
        {
            if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
            {
                tids_buf[cntr] = entry->d_name;
                cntr++;
            }
        }
        closedir(dir_fd);

        printf("TIDs: ");
        for(int i = 0; i < cntr - 1; i++)
            printf("%s, ", tids_buf[i]);

        printf("%s\n", tids_buf[cntr - 1]);
    }

    printf("TGID: %d\n", proc_info.tgid);

    printf("UID: %d, GID: %d\n", proc_info.ruid, proc_info.rgid);

    // number of process's groups
    if(proc_info.supgid != NULL)
        printf("Groups: %s\n", proc_info.supgid);

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
            __bits[__CPU_SETSIZE / __NCPUBITS];            just to remember array and number of bits
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
        printf("------------------------------------------USAGE-----------------------------------------\n");

        printf("User-mode CPU time accumulated by process:                        %lld\n", proc_info.utime);
        printf("Kernel-mode CPU time accumulated by process:                      %lld\n", proc_info.stime);
        printf("Cumulative user-mode CPU time of process and reaped children:     %lld\n", proc_info.cutime);
        printf("Cumulative kernel-mode CPU time of process and reaped children:   %lld\n", proc_info.cstime);
        printf("Total virtual memory in pages:                                    %ld\n",  proc_info.size);
        printf("Resident non-swapped memory in pages:                             %ld\n",  proc_info.resident);
        printf("Shared memory:                                                    %ld\n",  proc_info.share);

        if(proc_info.rss_rlim == RLIM_INFINITY)
            printf("Maximum resident set size:                                        Unlimited\n");
        else
            printf("Maximum resident set size:                                        %lu\n",  proc_info.rss_rlim);

        printf("Text resident set:                                                %ld\n",  proc_info.trs);
        printf("Library resident set:                                             %ld\n",  proc_info.lrs);
        printf("Data and stack resident set in pages:                             %ld\n",  proc_info.drs);
        printf("Page reclaims (soft page faults):                                 %ld\n",  proc_info.min_flt);
        printf("Page faults (hard page faults):                                   %ld\n",  proc_info.maj_flt);
        printf("Swaps:                                                            %ld\n",  proc_info.vm_swap);
        printf("Quit signal:                                                      %d\n",   proc_info.exit_signal);

#ifdef SIGNAL_STRING 

        printf("Pending signals:                                                  %s\n",   proc_info.signal);
        printf("Blocked signals:                                                  %s\n",   proc_info.blocked);
        printf("Ignored signals:                                                  %s\n",   proc_info.sigignore);
        printf("Caught signals:                                                   %s\n",   proc_info.sigcatch);
        printf("Per task pending signals:                                         %s\n",   proc_info._sigpnd);

#else
        printf("Pending signals:                                                  %lld\n",   proc_info.signal);
        printf("Blocked signals:                                                  %lld\n",   proc_info.blocked);
        printf("Ignored signals:                                                  %lld\n",   proc_info.sigignore);
        printf("Caught signals:                                                   %lld\n",   proc_info.sigcatch);
        printf("Per task pending signals:                                         %lld\n",   proc_info._sigpnd);
#endif

        printf("========================================================================================\n");

    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                                 LIMITS INFO                                                    //
    if(flags & FLAG_LIMITS)
    {
        struct rlimit rm;
        printf("---------------------LIMITS---------------------------------SOFT-----HARD---------------\n");

        // Limits masks
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

        // Terminal limit messages
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

        // Buffer all limit messages will be written to. Maximum lenght of such string = 45 + 20 + 1 + 1 + 1 + 20
        // where 45 stands for limit message and spaces up to soft limit; 20 - soft limit itself; 1 - space;
        // 1 - '/' symbol; 1 - again space; 20 - hard limit
        char limit_buf[sizeof("Process's virtual memory maximum size:       01234567890123456789 / 01234567890123456789\n")];
        char* offset = NULL;
        
        for(int i = 0; i < LIMITS_NUM; i++)
        {
            offset = limit_buf;

            // Getting particular limit
            if(prlimit(proc_id, limits[i], NULL, &rm) < 0)
            {
                perror("getrlimit");
                result = -1;
                continue;
            }

            // And printing it
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
        printf("------------------------------------NAMESPACE INODES------------------------------------\n");

        // all namespaces except control group namespace

        const char* namespace_type[6] =
        {
            "ipc:",
            "mnt:",
            "net:",
            "pid:",
            "user:",
            "uts:"
        };

        for(int i = 0; i < NUM_NS; i++)
            printf("%-7s[%ld]\n", namespace_type[i], proc_info.ns[i]);


        // cgroup namespace
        char buf[sizeof("cgroup:[0123456789]")];                        // longest namespace inode info string
        char ns_str[sizeof("/proc//ns/cgroup") + MAX_PID_LENGHT];       // max size of string
        memset(buf, 0, sizeof(buf));

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

        cap_t caps_self = cap_get_pid(getpid());
        if (caps == NULL) {
            perror("cap_get_pid");
            result = -1;
        }

        // I couldn't find better way to obtain information about ambient and bounding sets capabilities
        // of a particular process. Just assigning to THIS process the same capabilities
        cap_set_proc(caps);

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

        // Granting THIS process capabilities it started with
        cap_set_proc(caps_self);
        cap_free(caps_self);
        cap_free(caps);

        printf("========================================================================================\n");
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                                EXTENDED INFO                                                   //

    if(flags & (FLAG_EXTEND | FLAG_ENVIR))
    {

        if(flags & FLAG_EXTEND)
        {
            printf("----------------------------------------EXTENDED----------------------------------------\n");

            printf("Nice level:                                                       %ld\n", proc_info.nice);
            printf("Total number of pages:                                            %ld\n", proc_info.size);
            printf("Dirty pages:                                                      %ld\n", proc_info.dt);
            printf("Number of pages in virtual memory:                                %lu\n", proc_info.vsize);
            printf("Size of pages in virtual memory:                                  %lu\n", proc_info.vm_size);
            printf("Locked pages in Kbytes:                                           %lu\n", proc_info.vm_lock);
            printf("Anonymous memory size in Kb:                                      %lu\n", proc_info.vm_rss_anon);
            printf("File-backed memory size in Kb:                                    %lu\n", proc_info.vm_rss_file);
            printf("Shared memory size as Kb:                                         %lu\n", proc_info.vm_rss_shared);
            printf("Data size as Kb:                                                  %lu\n", proc_info.vm_data);
            printf("Stack size as Kb:                                                 %lu\n", proc_info.vm_stack);
            printf("Executable size as Kb:                                            %lu\n", proc_info.vm_exe);
            printf("Library size as Kb:                                               %lu\n", proc_info.vm_lib);
            printf("Real user name:                                                   %s (%d)\n", proc_info.ruser, proc_info.ruid);
            printf("Effective user name:                                              %s (%d)\n", proc_info.euser, proc_info.euid);
            printf("Saved user name:                                                  %s (%d)\n", proc_info.suser, proc_info.suid);
            printf("Real group name:                                                  %s (%d)\n", proc_info.rgroup, proc_info.rgid);
            printf("Effective group name:                                             %s (%d)\n", proc_info.egroup, proc_info.egid);
            printf("Saved group name:                                                 %s (%d)\n", proc_info.sgroup, proc_info.sgid);
            printf("Filesystem group name:                                            %s\n", proc_info.fgroup);
            printf("Full device number of controlling terminal (TTY):                 %d\n", proc_info.tty);
            printf("Terminal process group id:                                        %d\n", proc_info.tpgid);
            printf("OOM score:                                                        %d\n", proc_info.oom_score);
            printf("OOM adjustment:                                                   %d\n", proc_info.oom_adj);

            printf("========================================================================================\n");
        }

        if(flags & FLAG_ENVIR)
        {
            printf("--------------------------------------ENVIRONMENT---------------------------------------\n");

            if(proc_info.environ == NULL)
                printf("No information about environment\n");
            else
            {   
                int i = 0;
                while(proc_info.environ[i])
                {
                    printf("%s\n", proc_info.environ[i]);
                    i++;
                }
            }

        }

    }


    return result;
}