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
    }
  closedir(watchDIR);
  
  return 0;
}