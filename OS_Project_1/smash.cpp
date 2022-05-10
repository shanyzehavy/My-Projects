/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"
#include "job.h"
#include <queue>
#define MAX_LINE_SIZE 80
#define MAXARGS 20
#define MAX_CMD 50
using namespace std;

char* L_Fg_Cmd;
list<job> list_jobs; 			//This represents the list of jobs 
queue<string> cmd_history; 		//
char lineSize[MAX_LINE_SIZE]; 	//The entire line of command
size_t cmd_ser_num = 1; 		//Set the serial num of the commands
job cur_fg_job;
extern int ex_err;
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE]; 	 
    cur_fg_job.pid = -1; //Initalize in case there's not running process in fg
      
	/************************************/

	/************************************/
	// Init globals 
	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
	if (L_Fg_Cmd == NULL) 
	{
		exit (-1); 
	}
	L_Fg_Cmd[0] = '\0';

	/************************************/

	/************************************/
	// Signal Handler

	struct sigaction st_siginit, st_sigtstp; 	 //Create sigactions variables for stop (ctrl z) and terminiate (ctrl c)
	st_siginit.sa_handler = &sig_init;		 	 //Set handler member of siginit variable with sig_init function (in signals.cpp)
	st_sigtstp.sa_handler = &sig_tstp;		 	 //Set handler member of sigtstp variable with sig_tstp function (in signals.cpp)
	st_siginit.sa_flags = SA_RESTART; 		 	 
	st_sigtstp.sa_flags = SA_RESTART; 		 
	sigaction(SIGINT, &st_siginit, NULL);		 //Using sigaction func to use the smash's "ctrl c"
    sigaction(SIGTSTP, &st_sigtstp, NULL);

	
    while (1) //Smash process
    {
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin); //Getting input from user
		strcpy(cmdString, lineSize);    	
		cmdString[strlen(lineSize)-1]='\0';
		//Handle history queue:
		if(cmd_history.size() == MAX_CMD)
		{
			cmd_history.pop(); //Remove element from queue in case of more than 50 cmd
		}
		cmd_history.push((string)cmdString); //Add this command to history queue


		/*//Background command:	
	 	if(!BgCmd(lineSize,list_jobs)) 
	 	{
	 		continue;
	 	} */

		//Perform built in command/ call exeternal command function
		ExeCmd(list_jobs, lineSize, cmdString, &cmd_history); 
		update_list(list_jobs);
		if (ex_err == -1) 
		{
			list_jobs.pop_back(); 
		}

		
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
    return 0;
}