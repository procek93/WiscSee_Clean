#define _XOPEN_SOURCE 500
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <ftw.h>
#include <unistd.h>

#define READ_REQUEST_SIZE        1024 * 4
/* directory names */

char * my_strings[] = {"loc_root", "loc_dirA", "loc_dirB", "loc_dirC", "loc_dirD", "loc_dirE", "loc_dirF", "loc_dirG", "loc_dirH", "loc_dirI", "loc_dirJ"};

int cmd_rmrf( char * path);

main()
{

    FILE *f;
    DIR * dir = opendir(my_strings[0]);
    int status = 0;

    int loop_count = 1000;

    int x;
    int input_fd;

    ssize_t ret_in;
    char buffer[READ_REQUEST_SIZE];      

    if ( dir )
    {
        /* we make a brass assumption that the directory exists */
        chdir(my_strings[0]);

        printf("walking through directories\n");
        for(x = 1; x < 11; x++)
        {
            /* we also just assume all the other directories exist since we run the prior workload */
            chdir(my_strings[x]);
 
            /* Create input file descriptor */
            input_fd = open ("large_file.txt", O_RDONLY);
            if (input_fd == -1) {
                    perror ("open");
                    return;
            }

            while((ret_in = read (input_fd, &buffer, READ_REQUEST_SIZE)) > 0)
            {
                printf("reading file...\n");
            }

            close(input_fd);

            chdir("..");
        }

    }
    else if (ENOENT == errno)
    {
        return;
    }
    else
    {
        return;
    }
}