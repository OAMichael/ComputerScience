#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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


int main(void)
{
    DIR* dir_fd = opendir(".");

    if(!dir_fd)
    {
        perror("opendir");
        return 1;
    }

    struct dirent* entry;

    while ((entry = readdir(dir_fd)) != NULL)
    {
        char entry_type = dtype_char(entry->d_type);
        if(entry_type == '?')
        {
            struct stat sb;
            if(lstat(entry->d_name, &sb) == 0)
                entry_type = mode_char(sb.st_mode);
        }
        printf("%c %s\n", entry_type, entry->d_name);
    }

    closedir(dir_fd);
    return 0;
}

