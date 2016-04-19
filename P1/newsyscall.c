#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h>
#include<linux/unistd.h>
#include<linux/cred.h>
#include<linux/string.h>
#include<linux/list.h>
#include <linux/uaccess.h>	//kernel--user

MODULE_LICENSE("Dual BSD/GPL");

struct prinfo{
		pid_t parent_pid;          //process id of parent
		pid_t pid;								 //process id
		pid_t first_child_pid;     //pid of youngest child
		pid_t next_sibling_pid;    //pid of older sibling
		long state;                //current state of process
		long uid;                  //user id of process owner
		char comm[64];             //name of program executed
		int depth;
};

#define __NR_hellocall 356
#define MAX_PROCESS_NUM 2000

int i,k;
int CURRENT_PROCESS_NUM = 0;        //已经读取到的process的数量
extern struct task_struct init_task;
struct task_struct *task;       //用来读取某个进程的task_struct
struct task_struct *tmp_task;
struct prinfo tmp[MAX_PROCESS_NUM];
int T = 1;

static int (*oldcall)(void);
void dfs(int s, struct prinfo *bf, int d)
{
	int j;
	tmp[T] = bf[s];
	tmp[T].depth = d;
	T++;
	for(j = 0;j < CURRENT_PROCESS_NUM;j++)
		if(bf[j].parent_pid==bf[s].pid)
			dfs(j,bf,d+1);
}


static int ptree(struct prinfo *buf,int *nr)
{
	printk("this is my android pstree syscall\n");
	//buf = (struct prinfo *)malloc(sizeof(struct prinfo) * MAX_PROCESS_NUM);

	//read_lock(&tasklist_lock);
	for_each_process(task)
	{
		//printk("get in!!!\n");
		//test
		if(task == &init_task)
		{
			printk("get it!\n");
			printk(task->comm);
			printk("\n");
		}
		tmp_task = task->parent;
		//printk("1 place~\n");
		buf[CURRENT_PROCESS_NUM].pid = task->pid;
		//printk("2 place~\n");

		//printk("3 place~\n");
		buf[CURRENT_PROCESS_NUM].parent_pid = (tmp_task != NULL)?tmp_task->pid:0;
		buf[CURRENT_PROCESS_NUM].state = task->state;
		strcpy(buf[CURRENT_PROCESS_NUM].comm,task->comm);
		buf[CURRENT_PROCESS_NUM].uid = task->cred->uid;
		//printk("4 place~\n");
		//get the first child pid
		buf[CURRENT_PROCESS_NUM].first_child_pid = (list_empty(&task->children))?0:list_entry((&task->children)->next,struct task_struct,sibling)->pid;
		//printk("5 place~\n");
		CURRENT_PROCESS_NUM ++;
		printk("%d, %s, %d\n",CURRENT_PROCESS_NUM,buf[CURRENT_PROCESS_NUM-1].comm,buf[CURRENT_PROCESS_NUM-1].pid);
	}//get the system process infomation
	//read_unlock(&tasklist_lock);
	*nr = CURRENT_PROCESS_NUM;
	for(i = 0;i < CURRENT_PROCESS_NUM;i++)
	{
		if(buf[i].parent_pid == 0)
		{
			printk("%s,%d,%ld,%d,%d\n",buf[i].comm,buf[i].pid,buf[i].state,buf[i].parent_pid,buf[i].uid);
			dfs(i,buf,1);
		}
	}//for the init_task to do dfs
	for(i = 0;i < CURRENT_PROCESS_NUM;i++)
		buf[i] = tmp[i];

	//add the init_task
	buf[0].pid = init_task.pid;
	buf[0].parent_pid = 0;
	buf[0].state = init_task.state;
	strcpy(buf[0].comm,init_task.comm);
	buf[0].uid = init_task.cred->uid;
	buf[0].first_child_pid = (list_empty(&(init_task.children)))?0:list_entry((&(init_task.children))->next,struct task_struct,sibling)->pid;
	for(i = 1;i < CURRENT_PROCESS_NUM;i++)
	{
		for(k = i+1;k<CURRENT_PROCESS_NUM;k++)
			if(buf[k].parent_pid == buf[i].parent_pid)
			{
				buf[i].next_sibling_pid = buf[k].pid;
				break;
			}
	}
	return 0;

}

static int addsyscall_init(void)
{
	long *syscall = (long*)0xc000d8c4;
	oldcall = (int(*)(void))(syscall[__NR_hellocall]);
	syscall[__NR_hellocall] = (unsigned long )ptree;
	printk(KERN_INFO "module load!\n");
	return 0;
}

static void addsyscall_exit(void)
{
	long *syscall = (long*)0xc000d8c4;
	syscall[__NR_hellocall] = (unsigned long )oldcall;
	printk(KERN_INFO "module exit!\n");
}
module_init(addsyscall_init);
module_exit(addsyscall_exit);
