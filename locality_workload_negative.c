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

    int x, y;
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

            for(y = 0; y < 50; y++)
            {
                /* 50 in place overwrites to trigger garbage collections */
                /* do in-place overwrites across the large file */
                status = in_place_overwrite("large_file.txt");
                if( status == -1 )
                {
                    printf("in place overwrite failed\n");
                }
            }

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

int in_place_overwrite(char * file_name)
{
    FILE *f;

    int quarter_block = 1024;
    int offset_multiplier = 0;
    int result;
    int x;

    if( file_name == NULL)
    {
        return -1;
    }

    f = fopen(file_name, "r+");

    if( f == NULL)
    {
        printf("ERROR: COULD NOT OPEN FILE\n");
        return -1;
    }


    while( fseek(f, quarter_block * offset_multiplier, SEEK_SET) == 0 )
    {
        printf("in place overwrite occuring...\n");

        for(x = 0; x < quarter_block; x++)
        {
            fprintf(f, "u"); 
        }

        offset_multiplier = offset_multiplier + 2;

    }

    offset_multiplier = 1;

    while( fseek(f, quarter_block * offset_multiplier, SEEK_SET) == 0 )
    {
        printf("in place overwrite occuring...\n");

        for(x = 0; x < quarter_block; x++)
        {
            fprintf(f, "w"); 
        }

        offset_multiplier = offset_multiplier + 2;

    }


    fclose(f);

    return 0;
}