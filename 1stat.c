#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>


char* MyCtime(struct timespec* time);  // My own ctime() version which prints nanoseconds and shows timezone information 
void FMode(struct stat* ptr);          // Function which prints file mode
char* perms(const int bytes);          // Function returning string of permissions of file


int main(int argc, char *argv[])
{
   struct stat sb;                     // Creating stat structure
   char* string;                       

   if (argc != 2) {
       fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);  // Failure in case of insufficient amount of arguments
       exit(EXIT_FAILURE);
   }

   if (lstat(argv[1], &sb) == -1) {
       perror("lstat");
       exit(EXIT_FAILURE);
   }

   printf("File name:                %s\n", argv[1]);

   printf("ID of containing device:  [%lx,%lx] (hexadecimal)\n",
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

   string = perms(sb.st_mode & 1023);                  // permissions string
   printf("Permissions:              %s\n", string);
   free(string);                                       // free memory

   tzset();
   string = MyCtime(&sb.st_ctim);                      // ctime string
   printf("Last status change:       %s", string);
   free(string);                                       // free memory

   string = MyCtime(&sb.st_atim);                      // atime string
   printf("Last file access:         %s", string);
   free(string);                                       // free memory
   
   string = MyCtime(&sb.st_mtim);                      // mtime string 
   printf("Last file modification:   %s", string);
   free(string);                                       // free memory

   exit(EXIT_SUCCESS);
}



char* MyCtime(struct timespec* time)
{

    struct tm* ttime = localtime(&(time->tv_sec));
    long int nsec = time->tv_nsec;

    char* str = (char*)calloc(80, sizeof(char));        // allocation of needed memory
    int i = 0;
    switch(ttime->tm_wday)
    {
      case 0: sprintf(str, "%s", "Sun "); break;        // week day
      case 1: sprintf(str, "%s", "Mon "); break;
      case 2: sprintf(str, "%s", "Tue "); break;
      case 3: sprintf(str, "%s", "Wed "); break;
      case 4: sprintf(str, "%s", "Thu "); break;
      case 5: sprintf(str, "%s", "Fri "); break;
      case 6: sprintf(str, "%s", "Sat "); break;
      default: exit(EXIT_FAILURE);
    }

    switch(ttime->tm_mon)
    {
      case 0:  sprintf(str + 4, "Jan "); break;          // month
      case 1:  sprintf(str + 4, "Feb "); break;
      case 2:  sprintf(str + 4, "Mar "); break;
      case 3:  sprintf(str + 4, "Apr "); break;
      case 4:  sprintf(str + 4, "May "); break;
      case 5:  sprintf(str + 4, "Jun "); break;
      case 6:  sprintf(str + 4, "Jul "); break;
      case 7:  sprintf(str + 4, "Aug "); break;
      case 8:  sprintf(str + 4, "Sep "); break;
      case 9:  sprintf(str + 4, "Oct "); break;
      case 10: sprintf(str + 4, "Nov "); break;
      case 11: sprintf(str + 4, "Dec "); break;
      default: exit(EXIT_FAILURE);
    }

    if(ttime->tm_mday < 10)                             
    {
        sprintf(str + 8, " ");
        i++;
    }    

    sprintf(str + 8 + i, "%d ", ttime->tm_mday);         // month day

    sprintf(str + 11, "%02d:", ttime->tm_hour);
    sprintf(str + 14, "%02d:", ttime->tm_min);
    sprintf(str + 17, "%02d" , ttime->tm_sec);


    sprintf(str + 19, ".%09ld %d ", nsec, ttime->tm_year + 1900);
    if(timezone < 0)
    sprintf(str + 35, "+");
    if(timezone >= 0)
    sprintf(str + 35, "-");
    sprintf(str + 36, "%02d GMT\n", abs(timezone/3600));   

    return str;
}



void FMode(struct stat* ptr)
{
   switch (ptr->st_mode & S_IFMT) {
   case S_IFBLK:  puts("File type:                Block device");            break;
   case S_IFCHR:  puts("File type:                Character device");        break;
   case S_IFDIR:  puts("File type:                Directory");               break;
   case S_IFIFO:  puts("File type:                FIFO/pipe");               break;
   case S_IFLNK:  puts("File type:                Symlink");                 break;
   case S_IFREG:  puts("File type:                Regular file");            break;
   case S_IFSOCK: puts("File type:                Socket");                  break;
   default:       puts("File type:                unknown?");                break;
   }
}


char* perms(const int bytes)
{
  int i = 0;
  char* prmstr = (char*)calloc(9, sizeof(char));      // allocation of memory

  for(i = 0; i < 9; i += 3)                           // User, group and other permissions to read respectively
    if((bytes & (1 << (8 - i))) != 0)
      prmstr[i] = 'r';
    else
      prmstr[i] = '-';

  for(i = 1; i < 9; i += 3)                           // User, group and other permissions to write respectively
    if((bytes & (1 << (8 - i))) != 0)
      prmstr[i] = 'w';
    else
      prmstr[i] = '-';

  for(i = 2; i < 9; i += 3)                           // User, group and other permissions to execute respectively
    if((bytes & (1 << (8 - i))) != 0)
      prmstr[i] = 'x';
    else
      prmstr[i] = '-';

  return prmstr;
}
