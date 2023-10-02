#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

int main()
{
  char cwd[1024];
  getcwd(cwd,1024);
  
  DIR* watchDIR = opendir(cwd);
  struct dirent* entp;
  
  while((entp = readdir(watchDIR))!=NULL) {
    if(entp->d_type == DT_REG)
      printf("%s\n",entp->d_name);
    }
  closedir(watchDIR);
  
  return 0;
}