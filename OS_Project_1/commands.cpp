//		commands.c
//********************************************
#include "commands.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdbool>
#include <iostream>
#include <list>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <assert.h>
#include <unistd.h>
#define MAX_CMD 50
#define MAX_BUF 1000
#define SIGTEM_TIMEOUT 5
using namespace std;

char* prev_dir = NULL; 
extern job cur_fg_job;
int ex_err;

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(list<job>& list_jobs, char* lineSize, char* cmdString, queue<string>* cmd_history )
{
	char* cmd; 
	char* args[MAX_ARG];	   //An array of the command input
	char pwd[MAX_LINE_SIZE];
	char* cur_dir = NULL; 			   //Create a pointer to the absolute path of the current directory
	list<job> :: iterator it;  //Create an iterator to go trough list
	list<job> :: iterator it_temp;
	char const *delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	//bool illegal_cmd = false; // illegal command
    cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
   	//Count number of args:
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters);  //? NULL??
		if (args[i] != NULL) 
			num_arg++; 
	}

	update_list(list_jobs);

/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) 
	{
		update_list(list_jobs);
		cur_dir = get_current_dir_name()/*getcwd(pwd, MAX_LINE_SIZE)*/; //Save current directory
		if(cur_dir == NULL)
		{
			perror("smash error: > get_current_dir_name() error");
			return 0;
		}
		else if(args[2] != NULL)
		{
			cout << "smash error: > Too many arguments" << endl;
			return 0;
		}
		else if(args[1] == NULL)//No path given
		{
			cout << "smash error: > Invalid arguments" << endl;
			return 0;
		}
		else if(*args[1] == '-') //In case of cd - : move to previous folder in use
		{
			if(prev_dir == NULL)////++++++??++++++++making sure its null and not ""
			{
				cout << "smash error: > No previous directory" << endl;
				return 0;
			}
			else if(chdir(prev_dir) == 0)
			{
				//change prev to cur:
				char *tmp = cur_dir; 
				cur_dir   = prev_dir;
				prev_dir  = tmp;
				//cout << cur_dir << endl; // no need in prining it ?????
			}
			else 
			{
				cout << "smash error: > No such file or directory" << endl;
				return 0;
			}
		}
		else //In case input is according to format: change directory and update previous one:
		{
			if(chdir(args[1]) == 0) //Change dir is successful
			{
				//free(prev_dir);
				prev_dir = cur_dir;
				cur_dir = args[1];
				//cout << cur_dir << endl;
			}
			else // chdir error
			{
				cout << "smash error: > No such file or directory" << endl;
				return 0;

			}
		}
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		cur_dir = getcwd(pwd, MAX_LINE_SIZE); 
		if(cur_dir == NULL)
		{
			perror("smash error: > getcwd() error");
			return 0;
		}
		else if(args[1] != NULL)
		{
			cout << "smash error: > Too many arguments" << endl;
			return 0;
		}
		else
		{
			cout << pwd << endl;
		}
		
	}
	
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
		if(args[1] != NULL)
		{
			cout << "smash error: > Too many arguments" << endl;
			return 0;
		}
		else
		{
			string tmp;
			//char tmp[MAX_LINE_SIZE];
			int size_queue = cmd_history->size();
			for(int i=0 ; i<size_queue; i++)
			{	tmp=cmd_history->front();
				//strcpy(tmp,(cmd_history->front()).c_str());
				cmd_history->pop();
				cout << tmp << endl;
				cmd_history->push(tmp);
			}
		}
	}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{
		update_list(list_jobs);
		if(args[1] != NULL)
		{
			cout << "smash error: > Too many arguments" << endl;
			return 0;
		}
		it = list_jobs.begin(); //Assign an iterator to go through the list
		while(it != list_jobs.end())
		{
			it->print_job();
			it++;
		}	
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		if(args[1] != NULL) 
		{
			cout << "smash error: > Too many arguments" << endl;
			return 0;
		}
		cout << "smash pid is " << getpid() << endl; //Getpid is always successful
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		update_list(list_jobs);
		bool flag_found = false; //Indicates weather the required job with serial num was found
		job fg_job = *(list_jobs.begin());
		if(args[2] != NULL) //In case command is not of the type fg [num]
		{
			cout << "smash error: > Too many arguments" << endl;
			return 0;
		}
		if(list_jobs.empty())
		{
			cout << "smash error: > No active jobs" << endl;
			return 0;
		}
		else //In case list isnt empty - find job to move to fg
		{
			it = list_jobs.begin();
			it_temp = it;
			if (args[1] == NULL) //No parameters
			{
				if (list_jobs.size() == 1) //In case there is only one 
				{
					flag_found = true;
					it_temp = it; //Temp iterator for later on use
					fg_job = *it;
				}
				else
				{
					it++;
					while (it != list_jobs.end()) //Find job with max serial number
					{
						if(it->ser_num > fg_job.ser_num)
						{
							it_temp = it; //Temp iterator for later on use
							fg_job = *it;	

						}
						it++;
					}
					flag_found = true;
				}
			}
			else //fg command with parameters
			{
				it = list_jobs.begin();
				if (list_jobs.size() == 1) //In case there is only one 
				{
					if(it->ser_num == (size_t)(atoi(args[1])))
					{
						flag_found = true;
						it_temp = it; //Temp iterator for later on use
						fg_job = *it;
					}	
				}
				else 
				{
					while (it != list_jobs.end()) //Find job with required serial number
					{
						if(it->ser_num == (size_t)(atoi(args[1]))) //Args 1 is required serial number
						{
							fg_job = *it;
							it_temp = it;	
							flag_found = true;
							break;			
						}
						it++;
					}
				}

			}
			if(flag_found == true) //In case job found - move to foreground
			{
				cur_fg_job = fg_job;
				list_jobs.erase(it_temp);
				cout << fg_job.proc_name << endl;;
				//fg_job.print_job();
				if(fg_job.stopped) //In case job is stopped - change to continue, and wait
				{
					if(!kill(fg_job.pid, SIGCONT)) 
					{
						fg_job.stopped = false; 
						if (waitpid(fg_job.pid,NULL,WUNTRACED) == -1)
						{
							cur_fg_job.pid = (size_t)(-1);
							perror("smash error: > Waitpid() fail");
							return 0;
						}  
					}
					else
					{
						cur_fg_job.pid = -1;
						perror("smash error: > kill(SIGCONT) failed");
						return 0;
					}
				}
				else if(waitpid(fg_job.pid,NULL,WUNTRACED) == -1)//In case job was running (not stopped) - wait
				{
					cur_fg_job.pid = -1;
					perror("smash error: > Waitpid() fail");
					return 0;
				}	
			}
			else //In case job not found
			{
				cur_fg_job.pid = -1;
				cout << "smash error: > Job not found" << endl; 
				return 0;
			}
		}	
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
		update_list(list_jobs);
		if(args[2] != NULL)
		{
			cout << "smash error: > Too many arguments" << endl;
			return 0;
		}
		bool flag_found = false; //Indicates weather the required job with serial num was found
		job bg_job = *(list_jobs.begin());
		if(list_jobs.empty())
		{
			cout << "smash error: > No active jobs" << endl;
			return 0;
		}
		else //In case list isnt empty - find job to move to bg
		{
			it = list_jobs.begin();
			it_temp = it;
			if (args[1] == NULL) //No parameters
			{
				if (list_jobs.size() == 1) //In case there is only one 
				{
					if(bg_job.stopped == true)
					{
						flag_found = true;
						it_temp = it;
					}
				}
				else
				{
					it++;
					while (it != list_jobs.end()) //Find job with max serial number and STOP
					{
						if((it->ser_num > bg_job.ser_num) && (it->stopped == true))
						{
							//cout << "inside if check, bg_job.ser num =  " << bg_job.ser_num << endl;
							bg_job = *it;	
							flag_found = true;	
							it_temp = it;		
						}
						it++;
					}
				}
			}


			else //bg command with parameters
			{
				it = list_jobs.begin();
				if (list_jobs.size() == 1) //In case there is only one job
				{
					if(it->ser_num == (size_t)(atoi(args[1])))
					{
						if(bg_job.stopped == true)
						{
							flag_found = true;
							it_temp = it;
						}
					}	
				}
				else 
				{
					while (it != list_jobs.end()) //Find job with required serial number
					{
						if(it->ser_num == (size_t)(atoi(args[1]))) //Args 1 is required serial number
						{
							if (it->stopped == true)
							{
								bg_job = *it;	
								flag_found = true;
								it_temp = it;		
							}
							else
							{
								cout << "smash error: > job is not stopped" << endl; 
								return 0;
							}
							break;		
						}
						it++;
					}
				}
			}

			if(flag_found == true) //Move the required job (in case found) to foreground
			{
				if (!kill (bg_job.pid, SIGCONT))
				{
					it_temp->stopped = false;
					cout << bg_job.proc_name << endl; 
					//bg_job.print_job();
				}
				else 
				{
					perror("smash error: > SIGCONT failed");
					return 0;
				}	
			}
			else 
			{
				cout << "smash error: > Job not found" << endl;
				return 0;	
			}
		}
  		
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		if(args[2] != NULL) 
		{
			cout << "smash error: > Too many arguments" << endl;
			return 0;
		}
		else //Valid command:
		{
			int status;
            bool sigterm_succeed = false;
            clock_t time_first_sig_sent;
			if((args[1] != NULL)&&(!strcmp(args[1], "kill"))) //In case of command: quit kill
			{
				it = list_jobs.begin();
				while(it != list_jobs.end())
				{
					if(kill(it->pid, SIGTERM) == -1)
					{
                        perror("smash error: > kill(SIGTERM) failed");
                        return 0;
					}
                    //cout << "[" << (*job_it)->proc_num << "]" << (*job_it)->title_of_job << "– Sending SIGTERM...";
                    time_first_sig_sent = clock();
                    while (((double)(clock() - time_first_sig_sent) / CLOCKS_PER_SEC) < SIGTEM_TIMEOUT) 
                    {           
                        if (waitpid(it->pid, &status, WNOHANG) == -1) 
                        {
                            perror("smash error: > waitpid() failed ");
                            return 0;
                        }
                        else 
                        {
                        	 if (WIFSIGNALED(status) && WTERMSIG(status) == SIGTERM)
                            {
                                it = list_jobs.erase(it);
                                sigterm_succeed = true;
                                cout << "Done." << endl;
                                break;
                            }
                        }
                    }
					if (!sigterm_succeed) 
					{
                        cout << "(5 sec passed) Sending SIGKILL…";
                        if (!kill(it->pid, SIGKILL)) {
                            cout << " Done." << endl;
                            it = list_jobs.erase(it);
                        }
                        else
                            perror("smash error: > kill(SIGKILL) failed");
                    }
                   }
			}

			//Both caommnds - quit, quit kill will terminate smash:
			if(!kill(getpid(), SIGKILL))
			{
				cout << "Smash process terminated" << endl; 
			}
			else
			{
				perror("smash error: > kill()SIGKILL) failed ");
				return 0;
			}

			return 0;
		}
	}	

	/*************************************************/
	else if (!strcmp(cmd, "diff"))
	{
		if(args[3] != NULL) 
		{
			cout << "smash error: > Too many arguments" << endl;
			return 0;
		}
		else if((args[1] == NULL)||(args[2] == NULL))
		{
			cout << "smash error: > Too few arguments" << endl;
			return 0;
		}
		fstream f1, f2;
		char c1, c2;
		bool flag_cmp = true;
		f1.open(args[1], ios::in);
		f2.open(args[2], ios:: in);
		if((!f1.is_open())||(!f2.is_open()))
		{
			perror("smash error: > Failed to open files");
			f1.close();
			f2.close();
			return 0; 
		}
		/*if ((f1.get() == EOF) || (f2.get() == EOF)) //In case both files are empty - return files are equal (=0)
		{
			f1.close();
			f2.close();
			cout << "0" << endl;;
			
		}*/
		else //In case files are valid - start comparing:
		{
			c1 = 'a';
			c2 = 'a';
			while ((c1 != EOF) && (c2 != EOF))
			{
				c1 = f1.get();
				c2 = f2.get();
				if (c1 != c2)
				{
					cout << "1" << endl;
					flag_cmp = false;
					break;
				}
				
			}
			if(flag_cmp == true) 		//Print 0 in case files are the same
			{
				cout << "0" << endl;
			}
			f1.close();
			f2.close();
		}   		
	} 
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		update_list(list_jobs);
		bool flag_found = false; //Indicates weather the required job with serial num was found
		job kill_job;
		if(args[3] != NULL)
		{
			cout << "smash error: > Too many arguments" << endl;
			return 0;
		}
		else if(args[1][0] != '-')
		{
			cout << "smash error: > Invalid arguments" << endl;
			return 0;
		}
		else
		{
			it = list_jobs.begin();
			while (it != list_jobs.end()) //Find job with required serial number
			{
				if(it->ser_num == (size_t)(atoi(args[2]))) //Args 2 is required serial number
				{
					kill_job = *it;
					flag_found = true; 
					break;		
				}
				it++;
			}
			if(flag_found == true)
			{
				int sig_num = atoi(args[1])*(-1);
				if((sig_num < 1) || (sig_num > 32))
				{
					cout << "smash error: > Invalid arguments" << endl; 
					return 0;
				}
				else if(kill(kill_job.pid, sig_num)) //Error
				{
					
					cout << "smash error: > kill " << args[2] << " - cannot send signal" << endl;
					return 0;	 
				}
			}
			else
			{
				cout << "smash error: > kill " << args[2] << " - job does not exist" << endl; 
				return 0;
			}
    	}
	} 
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString, lineSize, list_jobs, num_arg);
	 	return 0;
	}
	/*if (illegal_cmd == true)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}*/

	//Insert new cmd into the cmd history queue
	/*if(cmd_history->size() == MAX_CMD)
	{
		cmd_history->pop(); //Remove element from queue in case of more than 50 cmd
	}
	cmd_history->push(lineSize);
	*/
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************

void ExeExternal(char *args[MAX_ARG], char* cmdString, char* lineSize, list<job>& list_jobs, int num_arg)
{
	
	int pid;
	update_list(list_jobs);
    switch(pid = fork()) 
	{
    	case -1: // error
        		perror("smash error: > Fork() failed");
        		break;
        case 0 : // Child Process
            	setpgrp();
            	ex_err = execvp(args[0],args); //Create the process in system
				perror("smash error: > exec() function failed"); //Only performed in case of execvp fail
				return;
				break;
				
		default: //Parent
				if (BgCmd(args, cmdString, list_jobs, pid, num_arg)) //In case the command is not a background command
				{
					cur_fg_job = job(cmdString/*lineSize*/, pid,0, 0, false); 
					//cur_fg_job.print_job();
					sleep(1);
					if (waitpid(pid,NULL,WUNTRACED) == -1)
						{
							perror("smash error: > Waitpid() fail");
						} 
				}
				break;
	} 
	
}

//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char *args[MAX_ARG],char* cmdString, list<job>& list_jobs, int pid, int num_arg )
{
	if (/*cmdString[strlen(cmdString)-1]*/*args[num_arg] == '&' ) //Check if command is background
	{
		sleep(1);
		cmdString[strlen(cmdString)-2] = '\0';	  
		job add_job(cmdString, pid, cmd_ser_num++, time(NULL), false); //Create a new job
		list_jobs.push_back(add_job); //Insert new job into jobs list
		update_list(list_jobs);
		//add_job.print_job();
		return 0;
	}
	return -1;
}

//***********************************************************

//Update the job list; update stop/ delete jobs that are finished
void update_list (list<job>& list_jobs)
{
    int status = -1;
    int check = 0;
    list<job>::iterator it = list_jobs.begin();
    while (it != list_jobs.end())
    {
        if ((it->pid > 0) && (check = waitpid(it->pid, &status, WNOHANG | WUNTRACED) != 0))
        {
        	if(check == -1)
        	{
        			perror("smash error: > waitpid() failed "); 
        			return;
        	}
            else 
            {
            	if(WIFSTOPPED(status))//Update'stopped' for each job
           		{
            		it->stopped = true;
               		it++;
               	}
               	else if(WIFCONTINUED(status))
               	{
               		it->stopped = false;
               		it++;
               	}
               	else
               	{
               		it=list_jobs.erase(it); //Delete job in case finished 
               	}
               	status = -1;
             }
         }
        else
        {
            it++;
        }
    }
}



//***********************************************************

