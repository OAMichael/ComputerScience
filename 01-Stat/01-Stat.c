#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>


//MyCtime() was made because strftime() does not print nanocesonds
//thus, MyCtime() corrects it
char* MyCtime(struct timespec* time);
void FMode(struct stat* ptr);


int main(int argc, char *argv[])
{
   struct stat sb;
   char* string;

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

   FMode(&sb);

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

   string = MyCtime(&sb.st_ctim);
   printf("Last status change:       %s", string);
   free(string);


   string = MyCtime(&sb.st_atim);
   printf("Last file access:         %s", string);
   free(string);

   string = MyCtime(&sb.st_mtim);
   printf("Last file modification:   %s", string);
   free(string);

   return 0;

}


char* MyCtime(struct timespec* time)
{

    struct tm* ttime = localtime(&(time->tv_sec));
    long int nsec = time->tv_nsec;

    char* str = (char*)calloc(32, sizeof(char));
    
    strftime(str, 20, "%F %T", ttime);
    sprintf(str + 19, ".%ld ", nsec);
    strftime(str + 30, 7, "%z\n", ttime);

    return str;
}


void FMode(struct stat* ptr)
{
   switch (ptr->st_mode & S_IFMT) 
   {
     case S_IFBLK:  puts("File type:                block device");            break;
     case S_IFCHR:  puts("File type:                character device");        break;
     case S_IFDIR:  puts("File type:                directory");               break;
     case S_IFIFO:  puts("File type:                FIFO/pipe");               break;
     case S_IFLNK:  puts("File type:                symlink");                 break;
     case S_IFREG:  puts("File type:                regular file");            break;
     case S_IFSOCK: puts("File type:                socket");                  break;
     default:       puts("File type:                unknown?");                break;
   }
}