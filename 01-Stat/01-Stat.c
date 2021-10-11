#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>


//my_ctime() was made because strftime() does not print nanocesonds
//thus, my_ctime() corrects it
int my_ctime(char* out_buf, size_t buf_len, const struct timespec* file_timestamp)
{
    //break UNIX timestamp into components
    struct tm* ttime = localtime(&(file_timestamp->tv_sec));

    // format string with date
    char yyyymmdd_hhmmss[sizeof("YYYY-mm-dd HH:MM:SS")];
    char tz_str[sizeof("+hhmm")];

    // format timezone string
    strftime(yyyymmdd_hhmmss, sizeof(yyyymmdd_hhmmss), "%F %T", ttime);
    strftime(tz_str, sizeof(tz_str), "%z", ttime);

    // build final result from string components
    return snprintf(out_buf, buf_len, "%s.%09ld %s", yyyymmdd_hhmmss, file_timestamp->tv_nsec, tz_str);
        
}


const char* fmode(unsigned mode)
{
    switch (mode & S_IFMT) 
    {
        case S_IFBLK:  return "block device";
        case S_IFCHR:  return "character device";
        case S_IFDIR:  return "directory";
        case S_IFIFO:  return "FIFO/pipe";
        case S_IFLNK:  return "symlink";
        case S_IFREG:  return "regular file";
        case S_IFSOCK: return "socket";
        }

     return "unknown?";
}


int main(int argc, char *argv[])
{
   struct stat sb;
   char string[sizeof("YYYY-mm-dd HH:MM:SS.nnnnnnnnn +hhmm")];

   if (argc != 2) 
   {
       fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
       return 1;
   }

   if (lstat(argv[1], &sb) == -1) 
   {
       perror("lstat");
       return 2;
   }

   printf("File name:                %s\n", argv[1]);

   printf("ID of containing device:  [%lx,%lx]\n",
        (long) major(sb.st_dev), (long) minor(sb.st_dev));

   printf("File type                 %s\n", fmode(sb.st_mode));

   printf("I-node number:            %ld\n", (long) sb.st_ino);

   printf("Mode:                     %lo (octal)\n",
           (unsigned long) sb.st_mode);

   printf("Link count:               %ld\n", (long) sb.st_nlink);
   printf("Ownership:                UID=%ld   GID=%ld\n",
           (long) sb.st_uid, (long) sb.st_gid);

   printf("Preferred I/O block size: %ld bytes\n",
           (long) sb.st_blksize);
   printf("File size:                %lld bytes\n",
           (long long) sb.st_size);
   printf("Blocks allocated:         %lld\n",
           (long long) sb.st_blocks);

   tzset();

   my_ctime(string, sizeof("YYYY-mm-dd HH:MM:SS.nnnnnnnnn +hhmm"), &sb.st_ctim);
   printf("Last status change:       %s\n", string);

   my_ctime(string, sizeof("YYYY-mm-dd HH:MM:SS.nnnnnnnnn +hhmm"), &sb.st_atim);
   printf("Last file access:         %s\n", string);

   my_ctime(string, sizeof("YYYY-mm-dd HH:MM:SS.nnnnnnnnn +hhmm"), &sb.st_mtim);
   printf("Last file modification:   %s\n", string);

   return 0;

}

