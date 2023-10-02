#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

int main()
{
    //cwd: current working directory
  char cwd[1024];
  getcwd(cwd,1024);
  
  //creating directory stream
  DIR* watchDIR = opendir(cwd);
  struct dirent* entp;
  
  //a single readdir returns the next entry in the directory(watchDIR)
  //when the entire directory is read, it will return NULL
  while((entp = readdir(watchDIR))!=NULL) {
    if(entp->d_type == DT_REG)
      printf("%s\n",entp->d_name);
      //call stat for each file
      //check the info: modification time for the file
      //save the info about the file that changed and leave a log
      //log file:name of the file, and time in which it changed
      
    }
  closedir(watchDIR);
  
  return 0;
}