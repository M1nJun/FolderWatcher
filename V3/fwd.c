#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <signal.h>

#define BUF_LEN 256

FILE *log_f;
int run = 1;

struct folder {
    char tag[16];
    char path[32];
};

void signalHandler(int signal) {
    run = 0;
}

int main() {
    pid_t pid = fork();
    if(pid == 0){

        char line[256];
        FILE *conf_file = fopen("/etc/fwd.conf","r");
        FILE* log_f = fopen("/var/log/fwd.log", "a");
        int numFolders = 0;
        while (fgets(line, sizeof(line), conf_file) != NULL) {
            numFolders++;
        }
        rewind(conf_file);
        int fd = inotify_init1(0);

        int *wds = (int*) malloc(numFolders*sizeof(int));

        struct folder *F = (struct folder*) malloc(numFolders*sizeof(struct folder));
        for(int i=0;i<numFolders;i++){ 
            fscanf(conf_file,"%s %s",F[i].tag,F[i].path);
        }

        for(int i=0;i<numFolders;i++){
            wds[i] = inotify_add_watch(fd, F[i].path, IN_MODIFY);
        }

        int epfd = epoll_create1(0);
        struct epoll_event event[3];
        event[0].data.fd = fd;
        event[0].events = EPOLLIN;
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event[0]);

        signal(SIGTERM, signalHandler);

        while(run){
            int nr_events = epoll_wait(epfd, event, 3, -1);
            if (nr_events < 0) {
                perror("epoll_wait");
                continue;
            }
            //I'm going to assume that the code after this point will be ran
            //after the epoll had an event
            char buf[BUF_LEN] __attribute__((aligned(4)));
            ssize_t len, i = 0;
            len = read(fd, buf, BUF_LEN);

            while (i < len){
                struct inotify_event *event = (struct inotify_event *) &buf[i];
                if (event->mask & IN_MODIFY){
                    char *f_name = event->name;
                    char *f_tag;
                    for(int j=0;j<numFolders;j++){
                        if(wds[j] == event->wd){
                            f_tag = F[j].tag;
                        }
                    }


                    char log[256];

                    time_t current_time;
                    struct tm *time_info;
                    char time_string[100];
                    time(&current_time);
                    time_info = localtime(&current_time);
                    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);

                    sprintf(log,"%s %s %s\n", f_tag, f_name, time_string);
                    fprintf(log_f,"%s",log);
                    fflush(log_f);
                }
                i+=sizeof(struct inotify_event) + event->len;
            }
        }
        printf("Received SIGTERM. Cleaning up.\n");
        fclose(log_f);
        free(wds);
        free(F);
        return 0;
    }
    else {
        FILE* pid_file = fopen("/run/fwd.pid", "w");
        fprintf(pid_file, "%dd", pid);
        fclose(pid_file);
        return 0;
    }
}