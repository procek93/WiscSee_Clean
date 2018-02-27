#define _XOPEN_SOURCE 500
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <ftw.h>
#include <unistd.h>

#define WRITE_REQUEST_SIZE        1024 * 2

/* directory names */

char * my_strings[] = {"loc_root", "loc_dirA", "loc_dirB", "loc_dirC", "loc_dirD", "loc_dirE", "loc_dirF", "loc_dirG", "loc_dirH", "loc_dirI", "loc_dirJ"};

int cmd_rmrf( char * path);

main()
{
    DIR * dir = opendir(my_strings[0]);
    int status = 0;

    int loop_count = 1000;


    if ( dir )
    {
        /* root exists */
        //printf( "ROOT FOUND\n" );
        //printf( "DELETING ROOT\n");
        
        status = cmd_rmrf( my_strings[0] );
        if ( status == -1 )
        {
            //printf( "ERROR: OTHER\n" );
            return;
        } 
        else
        {
            //printf( "DELETED ROOT\n" );

            while (loop_count != 0)
            {
                /* directory does not exist */
                status = create_file_hierarchy_workload();

                if( status == -1 )
                {
                    //printf("ERROR: COULD CREATE FILE HEIRARCHY WORKLOAD\n");
                    closedir(dir);
                    return;
                }

                /* now delete the entire heirarchy and start again */
                status = cmd_rmrf( my_strings[0] );

                if ( status == -1 )
                {
                    //printf( "ERROR: OTHER\n" );
                    closedir(dir);
                    return;
                }  

                loop_count--;
            }
        }
    }
    else if (ENOENT == errno)
    {
        while (loop_count != 0)
        {
            /* directory does not exist */
            status = create_file_hierarchy_workload();

            if( status == -1 )
            {
                //printf("ERROR: COULD CREATE FILE HEIRARCHY WORKLOAD\n");
                closedir(dir);
                return;
            }

            /* now delete the entire heirarchy and start again */
            status = cmd_rmrf( my_strings[0] );

            if ( status == -1 )
            {
                //printf( "ERROR: OTHER\n" );
                closedir(dir);
                return;
            }  

            loop_count--;
        }


    }
    else
    {
        closedir(dir);
        return;
        //printf( "ERROR: OTHER\n" );
    }

    closedir(dir);
    return;
}

int file_unlink( const char *file_path, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int status = remove(file_path);
    
    if( status )
        perror( file_path );

    return status;
}

int cmd_rmrf( char * path )
{
    return nftw(path, file_unlink, 64, FTW_DEPTH | FTW_PHYS);
}

int create_file_hierarchy_workload( void )
{
    int status = 0;
    struct stat st = {0};

    int x, y;

    if( stat(my_strings[0], &st) == -1 )
    {
       status = mkdir(my_strings[0], 0700);

       if( status == 0 )
       {
           chdir(my_strings[0]);

           /* create all of the directories */

           for(x = 1; x < 11; x++)
           {
                if( status == 0)
                {
                    if( (stat(my_strings[x], &st) == -1) )
                    {
                        status = mkdir(my_strings[x], 0700);
                    }
                    else
                    {
                        printf( "ERROR: DIRECTORY '%s' ALREADY EXISTS\n", my_strings[x]);
                        return -1;
                    }
                }
                else
                {
                    printf("ERROR: COULD NOT CREATE DIRECTORY '%s'\n", my_strings[x]);
                    return -1;

                }
           }

           /* write all of the files */
           for(y = 1; y < 11; y++)
           {
                /* walk into folder */
                chdir(my_strings[y]);

                /* create file pool */
                status = create_file_pools( WRITE_REQUEST_SIZE );

                if(status == -1)
                {
                    printf("ERROR: COULD NOT CREATE FILE POOL\n");
                    return -1;
                }
                
                /* walk up to parent */
                chdir("..");
           }
       }
       else
       {
            printf("ERROR: COULD NOT CREATE DIRECTORY '%s'\n", my_strings[0]);
            return -1;
       }
    }

    return 0;
}

/* in this workload, we aim to fill 200MB, so 20MB * 10 Directories */
int create_file_pools( int request_size_bytes )
{
    FILE *f;
    char filename_buf[20];

    int x, i, y, j;

    int status = 0;

    int total_directory_size_bytes = 10 * 1024 * 1024; //20MB ( 1kb * 1024 * 20)
    int total_number_of_files = total_directory_size_bytes / request_size_bytes;

    int total_number_of_files_divided = total_number_of_files / 2;

    int big_file_size = total_number_of_files_divided * request_size_bytes;
    /* request size is in bytes */
    if( request_size_bytes > total_directory_size_bytes )
    {
        /* can't have request bigger than our theoretical limit for our directory */
        return -1;
    }

    /* create all of the small files */
    for(x = 0; x < total_number_of_files; x++)
    {
        sprintf(filename_buf, "small_file_%d.txt", x);

        f = fopen(filename_buf, "w");

        if( f == NULL)
        {
            printf("ERROR: COULD NOT OPEN FILE\n");
            return -1;
        }

        /* write the request size number of bytes to the file */
        for(i = 0; i < request_size_bytes; i++)
        {
            fprintf(f, "A");
        }

        fclose(f);
    }

    #if 1

    /* delete the first half of the files (~10MB) */
    for(y = 0; y < total_number_of_files_divided; y++)
    {
        sprintf(filename_buf, "small_file_%d.txt", y);

        status = remove( filename_buf );

        if( status != 0)
        {
            printf("ERROR: COULD NOT DELETE FILE\n");
            return -1;
        }

    }

    /*EDIT HERE FOR GROUP BY DEATH TIME -- just remove the below */
    /*REST OF THIS IS TO MODIFY AGING BASED ON THE WORKLOAD*/
    /*FOR LOCALITY, REWRITE LARGE PARTS OF THE FILE A BUNCH */
    /*FOR REQUEST SIZE, JUST LEAVE IT BE */

    /* write a large file */

    f = fopen("large_file.txt", "w");

    if( f == NULL )
    {
        printf("ERROR: COULD NOT OPEN FILE\n");
        return -1;
    }

    for(j = 0; j < big_file_size; j++)
    {
        fprintf(f, "B");
    }
    
    fclose(f);
    #endif

    return 0;
}


