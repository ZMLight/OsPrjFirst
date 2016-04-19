#include "hello.h"

struct prinfo{
		pid_t parent_pid;          //process id of parent
		pid_t pid;		   //process id
		pid_t first_child_pid;     //pid of youngest child
		pid_t next_sibling_pid;    //pid of older sibling
		long state;                //current state of process
		long uid;                  //user id of process owner
		char comm[64];             //name of program executed
		int depth;
};


struct prinfo buf[3000];
int proNum;
int i,j;

int main(int argc,char *argv[]){
	int i = syscall(356,buf,&proNum);
	printf("The process num is %d\n", proNum);

	for(i = 0;i<proNum;i++)
	{
		for(j = 0;j<buf[i].depth;j++)
			printf("	");
		printf("%s,%d,%ld,%d,%d,%d,%d\n",buf[i].comm,buf[i].pid,buf[i].state,buf[i].parent_pid,buf[i].first_child_pid,buf[i].next_sibling_pid,buf[i].uid);
	}




	return 0;
}
