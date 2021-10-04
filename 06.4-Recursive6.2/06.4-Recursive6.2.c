#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

char dtype_char(unsigned dtype);
char mode_char(unsigned mode);
size_t PrintDir(const char* dir_name, int* nNestCtr);


int main(int argc, char* argv[])
{
    const char* dir_name = ".";
    
    if(argc == 2)
    {
        dir_name = argv[1];
    }

    int NestCounter = 0;
    int* ptr = &NestCounter; 

    PrintDir(dir_name, ptr);

    return 0;
}


size_t PrintDir(const char* dir_name, int* nNestCtr)
{
    DIR* dir_fd = opendir(dir_name);

    if(!dir_fd)
    {
        perror("opendir");
        return 1;
    }

    int fd = dirfd(dir_fd);
    struct dirent* entry;

    while ((entry = readdir(dir_fd)) != NULL)
    {
        char entry_type = dtype_char(entry->d_type);
        if(entry_type == '?')
        {
            struct stat sb;
            if(fstatat(fd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW) < 0)
            {
                perror("fstatat");
                return 2;
            }
            entry_type = mode_char(sb.st_mode);
        }

        printf("%c|  ", entry_type);

        if(*nNestCtr > 1)
            for (int i = 0; i < *nNestCtr - 1; i++)
                printf("|  ");

        if(*nNestCtr > 0)
            printf("|--");


        printf("%s\n", entry->d_name);
        if((entry_type == 'd') && !( ((strlen(entry->d_name) == 1) && (entry->d_name[0] == '.')) ||
                                     ((strlen(entry->d_name) == 2) && (entry->d_name[0] == '.') && (entry->d_name[1] == '.')) ))
        {   
            (*nNestCtr)++;
            chdir(entry->d_name);
            PrintDir(".", nNestCtr);
        }
    }

    closedir(dir_fd);

    if(*nNestCtr != 0)
    chdir("..");

    (*nNestCtr)--;

    return 0;
}


char dtype_char(unsigned dtype)
{
    switch (dtype)
    {
        case DT_BLK:    return 'b';
        case DT_CHR:    return 'c';
        case DT_DIR:    return 'd';
        case DT_FIFO:   return 'p';
        case DT_LNK:    return 'l';
        case DT_REG:    return '-';
        case DT_SOCK:   return 's';
    }
    return '?';
}


char mode_char(unsigned mode)
{
    switch (mode & S_IFMT)
    {
        case S_IFBLK:   return 'b';
        case S_IFCHR:   return 'c';
        case S_IFDIR:   return 'd';
        case S_IFIFO:   return 'p';
        case S_IFLNK:   return 'l';
        case S_IFREG:   return '-';
        case S_IFSOCK:  return 's';
    }
    return '?';
}