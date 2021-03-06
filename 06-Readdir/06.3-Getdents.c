#define _GNU_SOURCE
#define BUF_SIZE 65536

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "../Library/util.h"


struct linux_dirent64 
{
    ino64_t        d_ino;    /* 64-bit inode number */
    off_t          d_off;    /* 64-bit offset to next structure */
    unsigned short d_reclen; /* Size of this dirent */
    unsigned char  d_type;   /* File type */
    char           d_name[]; /* Filename (null-terminated) */
};


int main(int argc, char* argv[])
{
    const char* dir_name = ".";
    char buf[BUF_SIZE];
    
    if(argc == 2)
    {
        dir_name = argv[1];
    }

    int dir_fd = open(dir_name, O_RDONLY | O_DIRECTORY);

    if(!dir_fd)
    {
        perror("open");
        return 1;
    }

    struct linux_dirent64* entry = NULL;
    long int nread = 0;

    int pos = 0;
    while(1)
    {
        nread = getdents64(dir_fd, buf, BUF_SIZE);

        if(nread == -1)
        {
            perror("getdents64");
            close(dir_fd);
            return 2;
        }

        if(nread == 0)
            break;

        while(pos < nread) 
        {
            entry = (struct linux_dirent64*) (buf + pos);

            if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            {
                pos += entry->d_reclen;
                continue;
            }

            if(entry->d_type == DT_UNKNOWN)
            {
                struct stat sb;
                if(fstatat(dir_fd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW) < 0)
                {
                    perror("fstatat");
                }
                else
                    entry->d_type = IFTODT(sb.st_mode);
            }

            printf("%c|", dtype_char(entry->d_type));
            printf("%s\n", entry->d_name);
            pos += entry->d_reclen;
        }
    }

    close(dir_fd);
    return 0;
}
