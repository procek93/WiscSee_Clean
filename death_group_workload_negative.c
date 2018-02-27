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

#define WRITE_REQUEST_SIZE        1024
/* directory names */

char * my_strings[] = {"loc_root", "loc_dirA", "loc_dirB", "loc_dirC", "loc_dirD", "loc_dirE", "loc_dirF", "loc_dirG", "loc_dirH", "loc_dirI", "loc_dirJ"};

int cmd_rmrf( char * path);

main()
{

    char cwd[1024];
    
    FILE *f;
    DIR * dir = opendir(my_strings[0]);
    int status = 0;

    int loop_count = 1000;

    int x, y, z, i;
    int input_fd;

    ssize_t ret_in;

    char filename_buf[20];  

    int deleted_count = 0;
    int incrementer = 0;

    if ( dir )
    {
        /* we make a brass assumption that the directory exists */
        chdir(my_strings[0]);

            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                fprintf(stdout, "Current working dir: %s\n", cwd);
            }
            else
            {
                perror("getcwd() error");
                return;
            }

        printf("walking through directories for death deletions\n");
        for(x = 2; x < 11; x++)
        {
            deleted_count = 0;
            incrementer = 0;
            /* we also just assume all the other directories exist since we run the prior workload */
            chdir(my_strings[x]);

            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                fprintf(stdout, "Current working dir: %s\n", cwd);
            }
            else
            {
                perror("getcwd() error");
                return;
            }

            /* delete 20 logically contiguous files */
            while(deleted_count != 20)
            {
                if( incrementer % 2 == 0)
                {
                    sprintf(filename_buf, "small_file_%d.txt", incrementer);

                    status = remove(filename_buf);

	                if( status )
	                {
	                    perror( filename_buf );
	                }
                    else
                    {
                        deleted_count++;
                        printf("removed %s\n", filename_buf);
                    }

                }

                incrementer++;
            }

            chdir("..");
        }

        /* write new files in each directory */
        for(x = 2; x < 11; x++)
        {
            /* we also just assume all the other directories exist since we run the prior workload */
            chdir(my_strings[x]);

            f = fopen("bigger_file.txt", "w");

            if( f == NULL)
            {
                printf("ERROR: COULD NOT OPEN FILE\n");
                return -1;
            }

            /* write the request size number of bytes to the file */
            for(i = 0; i < WRITE_REQUEST_SIZE * 10; i++)
            {
                fprintf(f, "A");
            }

            fclose(f);

            chdir("..");
        }

    }
    else if (ENOENT == errno)
    {
        closedir(dir);
        return;
    }
    else
    {
        closedir(dir);
        return;
    }

    closedir(dir);
    return;
}
