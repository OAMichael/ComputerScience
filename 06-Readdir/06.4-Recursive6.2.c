#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../Library/util.h"


size_t print_dir(const int fd, unsigned level)
{
    DIR* dir_fd = fdopendir(fd);

    if(!dir_fd)
    {
        perror("fdopendir");
        return 1;
    }

    struct dirent* entry;

    while ((entry = readdir(dir_fd)) != NULL)
    {
        if(errno != 0)
        {
            perror("readdir");
            break;
        }

        if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;

        char entry_type = dtype_char(entry->d_type);
        if(entry_type == '?')
        {
            struct stat sb;
            if(fstatat(fd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW) < 0)
            {
                perror("fstatat");
            }
            else
                entry_type = mode_char(sb.st_mode);
        }

        printf("%c|  ", entry_type);

        if(level > 1)
            for (unsigned i = 0; i < level - 1; i++)
                printf("|  ");

        if(level > 0)
            printf("|--");

        printf("%s\n", entry->d_name);
        if((entry_type == 'd'))
        {   
            int nest_dirfd = openat(fd, entry->d_name, O_RDONLY | O_DIRECTORY);
            print_dir(nest_dirfd, level + 1);
            close(nest_dirfd);
        }
    }

    return 0;
}


int main(int argc, char* argv[])
{
    const char* dir_name = ".";
    
    if(argc == 2)
    {
        dir_name = argv[1];
    }

    unsigned level = 0;

    int fd = open(dir_name, O_RDONLY | O_DIRECTORY);

    print_dir(fd, level);

    close(fd);

    return 0;
}
