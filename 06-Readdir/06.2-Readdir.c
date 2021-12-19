#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "../Library/util.h"


int main(int argc, char* argv[])
{
    const char* dir_name = ".";
    
    if(argc == 2)
    {
        dir_name = argv[1];
    }

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
        printf("%c| %s\n", entry_type, entry->d_name);
    }

    closedir(dir_fd);
    return 0;
}
