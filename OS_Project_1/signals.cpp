// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"
using namespace std;

extern job cur_fg_job;
extern list<job> list_jobs;


//Ctrl z - SIGTSTP
void sig_tstp(int sig)
{
	if(cur_fg_job.pid != (size_t) -1)//Only in case there's a running process in fg
	{
		//Terminate the smash
		if(cur_fg_job.pid == (size_t)getpid())
		{
			return;
		}
		if(kill(cur_fg_job.pid, SIGTSTP) == -1)
		{
			perror("smash error: > SIGTSTP failed");
		}
		cur_fg_job.stopped = true;
		cur_fg_job.begin_time = time(NULL);
		cur_fg_job.ser_num = cmd_ser_num++; //Once a job is re-entre the jobs list it has a new ser_num
		list_jobs.push_back(cur_fg_job);
		cur_fg_job.pid = -1; //There's no running process in fg
	}
	cout << endl;

}



//Ctrl c - SIGINT
void sig_init(int sig)
{
	if(cur_fg_job.pid != (size_t)-1)//Only in case there's a running process in fg
	{
		//Terminate the smash
		if(cur_fg_job.pid == (size_t)getpid())
		{
			cout << endl;
			return;
		}
		if(kill(cur_fg_job.pid, SIGINT) == -1)
		{
			perror("smash error: > SIGINT failed");
		}
		cur_fg_job.pid = -1; //There's no running process in fg
				//???? do we need to delete cur_fg_job or reset it somehow?
	}
	cout << endl;
	
}
