#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

int timeDiff (const timespec* lhs, const timespec* rhs)
{
    if (lhs->tv_sec < rhs->tv_sec)
        return 0;
    else
        return 1;
}

int main(const char * argv[])
{
    char * path = argv[1];
    //cwd: current working directory
    char cwd[1024];
    getcwd(cwd,1024);
    
    //creating directory stream
    DIR* watchDIR = opendir(cwd);
    struct dirent* entp;
    
    FILE* f = fopen("log.txt", "w");

    struct stat st_l;
    //infinite loop
    while(1){
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        sleep(300); //sleep for 5 minutes = 300 seconds

        //a single readdir returns the next entry in the directory(watchDIR)
        //when the entire directory is read, it will return NULL
        while((entp = readdir(watchDIR))!=NULL) {
            if(entp->d_type == DT_REG)
            printf("%s\n",entp->d_name);
            //call stat for each file
            struct stat st;
            int stat (path, &st);
            if (timeDiff(&ts, &st.st_mtim)){
                //question: should I combine d_name and the last modification time into a big string
                //and write it out to the log file?
                fprintf(f,)
            }


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