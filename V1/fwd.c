#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>

int timeDiff (const struct timespec* lhs, const struct timespec* rhs)
{
    if (lhs->tv_sec < rhs->tv_sec)
        return 0;
    else
        return 1;
}

int main(const char * argv[])
{
    char * path = argv[1];
    //creating directory stream
    DIR* watchDIR = opendir(path);
    struct dirent* entp;
    
    FILE* f = fopen("log.txt", "w");

    struct stat st_l;
    //infinite loop
    while(1){
        time_t now = time(NULL);

        sleep(30); //sleep for 5 minutes = 300 seconds

        //a single readdir returns the next entry in the directory(watchDIR)
        //when the entire directory is read, it will return NULL
        while((entp = readdir(watchDIR))!=NULL) {
            char * f_name;
            if(entp->d_type == DT_REG){
                f_name = entp->d_name;
            }
            //call stat for each file
            struct stat st;
            stat(f_name, &st);
            if (now < st.st_mtime){
                //question: should I combine d_name and the last modification time into a big string
                //and write it out to the log file?
                char log[100];
                sprintf(log,"%s %s\n",f_name, ctime(&st.st_mtime));
                fprintf(f,"%s",log);
                fflush(f);
            }
        rewinddir(watchDIR);


            //question:to compare the two timespecs, I need at least one initial log in the txt file.
            //how do i initiallize that?


            //check the info: modification time for the file
            //save the info about the file that changed and leave a log
            //log file:name of the file, and time in which it changed
            
        }
        closedir(watchDIR);
    }
    return 0;
}