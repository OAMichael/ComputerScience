#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../Library/util.h"


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

