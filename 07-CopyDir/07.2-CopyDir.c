#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "../Library/util.h"


int main(int argc, char* argv[])
{
    const char* old_dir_name = ".";
    const char* new_dir_name = NULL;

    // if user entered not enough parameters
    if(argc < 3)
    {
        fprintf(stderr, "Usage: %s <old dir pathname> <new dir pathname>\n", argv[0]);
        return 1;
    }

    // if user entered enough parameters
    if(argc == 3)
    {
        old_dir_name = argv[1];
        new_dir_name = argv[2];
    }

    // obtaining DIR* pointer of directory program will copy from and opening it
    DIR* old_dir_fd = opendir(old_dir_name);

    // if directory with name <new_dir_name> already exists, then it will be
    // just opened. Otherwise, it will be created by mkdir()
    if(mkdir(new_dir_name, ALL_PERMS) < 0 && errno != EEXIST)
    {
        perror("mkdir");
        return -1;
    }

    // obtaining DIR* pointer of directory program will copy to and opening it
    DIR* new_dir_fd = opendir(new_dir_name);

    // summoning function which copies directory
    if(copy_dir(old_dir_fd, new_dir_fd) < 0)
    {
        fprintf(stderr, "\n\nFailure while copying directory or directory was copied partialy\n\n");
    }

    // closing both directories after work is done
    closedir(old_dir_fd);
    closedir(new_dir_fd);

    return 0;
}