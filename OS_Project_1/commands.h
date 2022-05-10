#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <queue>
#include "job.h"
#define MAX_LINE_SIZE 80
#define MAX_ARG 20

using namespace std;
//typedef enum { FALSE , TRUE } bool;
int BgCmd(char *args[MAX_ARG], char* cmdString, list<job>& list_jobs, int pid, int num_arg);
int ExeCmd(list<job>& list_jobs, char* lineSize, char* cmdString, queue<string>* cmd_history );
void ExeExternal(char *args[MAX_ARG], char* cmdString, char* lineSize, list<job>& list_jobs, int num_arg);
void update_list (list<job>& list_jobs); //Update the job list; update stop/ delete jobs that are finished

#endif

