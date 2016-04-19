#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
int main()
{
  int i = 1;
  pid_t  pid;
  /* fork another process */
  pid = fork();
  if (pid < 0)
  { /* error occurred */
    fprintf(stderr, "Fork Failed");
    return 1;
  }
  else if (pid == 0)
  { /* child process */
    printf("this is the Child process!\n");
    printf("5140309444 Parent, %d\n", getppid());
    printf("5140309444 Child, %d\n", getpid());
    printf("enter the ptree!\n");
    execl("/data/misc/ptree","",NULL);
    printf("exit the ptree!\n");
  }
  else
  { /* parent process */
  /* parent will wait for the child */
    wait(NULL);
    printf("this is the Parent process!\n");
    printf ("5140309444 Parent, %d\n",getpid());
  }
return 0;
}
