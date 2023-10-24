#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
FILE *log_f;

void* logWrite(char* log){
    pthread_mutex_lock(&mutex);
    fprintf(log_f,"%s",log);
    fflush(log_f);
    pthread_mutex_unlock(&mutex);
}

struct info {
            char tag[16];
            char path[16];
        };

//seperate function for mutex before printing out to the log file.

int timeDiff (const struct timespec* lhs, const struct timespec* rhs)
{
    if (lhs->tv_sec < rhs->tv_sec)
        return 0;
    else
        return 1;
}

void* watchDirectory(void *arg) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM);

    if (pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0) {
        perror("sigmask");
        return NULL;
    }
    struct info *I = (struct info*) arg;
    // char* path = "/home/minjun/Desktop/watchOne";
    
    //creating directory stream
    DIR* watchDIR = opendir(I->path);
    struct dirent* entp;
    

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
                //call stat for each file
                struct stat st;
                stat(f_name, &st);
                if (now < st.st_mtime){
                    //question: should I combine d_name and the last modification time into a big string
                    //and write it out to the log file?
                    char log[100];
                    sprintf(log,"%s %s %s\n", I->tag,f_name, ctime(&st.st_mtime));
                    logWrite(log);
                }
            }
        }
        rewinddir(watchDIR);
    }
    return NULL;
}

int main() {
    pid_t pid = fork();
    if(pid == 0){

        char **tags;
        char **paths;

        char line[256];
        FILE *conf_file = fopen("/etc/fwd.conf","r");
        FILE* log_f = fopen("/var/log/fwd.log", "a");
        int numThread = 0;
        while (fgets(line, sizeof(line), conf_file) != NULL) {
            numThread++;
        }
        
        pthread_t *tids = (pthread_t*) malloc(numThread*sizeof(pthread_t));
        struct info *I = (struct info*) malloc(numThread*sizeof(struct info));
        for(int i=0;i<numThread;i++){ 
            fscanf(conf_file,"%s %s",&I[i].tag,&I[i].path);
            pthread_create(&tids[i],NULL,watchDirectory,&I[i]);
        }

        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGTERM);

        int sig;
        sigwait(&set, &sig);

        pthread_mutex_lock(&mutex);
        for(int i=0;i<numThread;i++){
            pthread_cancel(tids[i]);
        }
        fclose(log_f);
        return 0;
    }
    else {
        FILE* pid_file = fopen("/run/fwd.pid", "w");
        fprintf(pid_file, "%ld", pid);
        fclose(pid_file);
        return 0;
    }
}