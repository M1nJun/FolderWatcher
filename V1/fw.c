#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

//argument here would be start or stop
int main(int argc,const char * argv[]){
    if(strcmp(argv[1], "start") == 0){
        pid_t pid = fork();
        if(pid == 0){
            //child process
            //run fwd
            char * fwdPath = "fwd";
            //command line arguments for new program
            char * cmdLine[] = {
                "fwd",
                argv[2]
            };
            //I don't have any environment variables I want to pass on
            //So, I'll just pass a NULL pointer
            int exec_result = execve(fwdPath, cmdLine, NULL);
            if (exec_result == -1) {
                perror("execve");
                exit(EXIT_FAILURE);
            }
        }
        else{
            //parent process
            //save pid in txt file
            FILE* f = fopen("pid.txt", "w");
            fprintf(f, "%ld", pid);
            fclose(f);
        }
    }
    else{
        //read pid from txt file and kill that process

        //question: Is it okay for pid to not be a pid_t type?
        pid_t pid;
        FILE* f = fopen("pid.txt", "rb");
        fscanf(f,"%ld",&pid);
        fclose(f);

        kill(pid,SIGKILL);
    }
}