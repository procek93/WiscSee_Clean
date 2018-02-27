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