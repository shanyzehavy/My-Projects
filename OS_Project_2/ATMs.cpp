#include "ATMs.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
#define MAX_LINESIZE 40
#define MAX_ARG 5
using namespace std;


extern list<Account> list_accounts;
extern ofstream logfile;	// Logfile is used to print activities log into	 
extern read_write list_lock;
extern pthread_mutex_t mutex_logfile;

/* This function sets the start routine of each atm thread */
void *atm (void *ptr_args)
{
	fstream f;
	Atm_thrd_args *thrd_args = (Atm_thrd_args*)ptr_args; 
	f.open(thrd_args->file_name, ios::in); 		// Open the atm activity file (its name is contained at ptr_args[1])
	if(!f.is_open())
	{
		perror("Failed to open files");
		exit (0); 
	}
	// ATM activity file had opened succesfully:
	char *line = (char*)malloc (sizeof(char*)*MAX_LINESIZE);
	if (line == NULL)
	{
		cout << "Failed to allocate memory" << endl;
		exit (0);
	}
	while(f.getline(line, MAX_LINESIZE))
	{
		atm_exec(line, list_accounts, thrd_args->atm_id ); 
	}
	// Before return - close all open files and free all memory:
	f.close();
	free (line); 
	return 0;
}





/* This function gets the string of the user's request, 
and handles it in order to execute, with relevant function.
in case of error returns -1, on succes returns 0 */
int atm_exec (char *line, list<Account>& list_accounts, int atm_id)
{
	char* req; 
	char* args[MAX_ARG];	  		    // An array of the command input
	char const *delimiters = " \t\n"; 	 
	int i = 0, num_arg = 0;
    req = strtok((char*)line, delimiters);
	if (req == NULL)
		return 0; 
   	args[0] = req;
	for (i=1; i<MAX_ARG; i++) // Count number of args
	{
		args[i] = strtok(NULL, delimiters);  //? NULL??
		if (args[i] != NULL) 
			num_arg++;                 // Notice that num_arg is actuallu num_arg-1 since arr starts from ZERO
	}
	if (!strcmp(req, "O"))
	{
		open_acc(args, list_accounts, atm_id);
	} 
	else if (!strcmp(req, "D"))
	{
		deposit_acc(args, list_accounts, atm_id );
	}
	else if (!strcmp(req, "W"))
	{
		withdraw_acc(args, list_accounts, atm_id );
	}
	else if (!strcmp(req, "B"))
	{
		balance_inq (args, list_accounts, atm_id);
	}
	else if (!strcmp(req, "Q"))
	{
		quit_acc (args, list_accounts, atm_id);
	}
	else if (!strcmp(req, "T"))
	{
		transfer(args, list_accounts, atm_id);
	}
	else // In case of invalid command
	{
		return -1;
	}
	usleep(100000);
	return 0;
}


/* This function opens a new account.
   Always returns 0 */ 
int open_acc(char *args[MAX_ARG], list<Account>& list_accounts, int atm_id) 
{ 
	list_lock.wrt_enter();
	if(find_account (args, list_accounts, atoi(args[1])) ==  list_accounts.end()) // In case there is no existing account with the required account number -> create it
	{
		Account new_acc(atoi(args[1]), atoi(args[2]), atoi(args[3])); // Create a new account
		list_accounts.push_back(new_acc);							  // Insert new acc into list
		list_accounts.sort(compare_accounts); 						  // Sorting the list
		pthread_mutex_lock(&mutex_logfile);
		logfile << atm_id+1 << ": New account id is " << args[1] << " with password " << args[2] << " and initial balance " << args[3] << endl;
		pthread_mutex_unlock(&mutex_logfile);
	}
	else
	{
		pthread_mutex_lock(&mutex_logfile);
		logfile << "Error " << atm_id+1 << ": Your transaction failed - account with the same id exists" << endl;
		pthread_mutex_unlock(&mutex_logfile);
	}
	sleep(1);
	list_lock.wrt_leave();
	return 0;
}


/* This function deposites to an account.
   Always returns 0 returns 0 */ 
int deposit_acc(char *args[MAX_ARG], list<Account>& list_accounts, int atm_id)
{
	list_lock.rd_enter();
	list<Account> :: iterator it = find_account(args, list_accounts, atoi(args[1])); //?? value not related in 4 arg
	if(it == list_accounts.end()) // In case the account does not exist
	{
		pthread_mutex_lock(&mutex_logfile);
		logfile << "Error " << atm_id+1 << ": Your transaction failed - account id " << args[1] << " does not exist" << endl;
		pthread_mutex_unlock(&mutex_logfile);
		sleep(1);
	}
	else // In case the account exists
	{
		it->rd_wrt.wrt_enter(); 		  // Lock the account
		if(it->password == atoi(args[2])) //In case the password is correct
		{
			it->amount = (it->amount + atoi(args[3]));
			pthread_mutex_lock(&mutex_logfile);
			logfile << atm_id+1 << ": Account "<< args[1] << " new balance is " << it->amount << " after " << args[3] << " $ was deposited" << endl;	
			pthread_mutex_unlock(&mutex_logfile);
		}
		else // Password was worng
		{
			pthread_mutex_lock(&mutex_logfile);
			logfile << "Error " << atm_id+1 << ": Your transaction failed - password for account id " << args[1] << " is incorrect" << endl;
			pthread_mutex_unlock(&mutex_logfile);
		}
		sleep(1);
		it->rd_wrt.wrt_leave();
	}
	list_lock.rd_leave();
	return 0;
}


/* This function withdraws from an account.
   Always returns 0 returns 0 */
int withdraw_acc(char *args[MAX_ARG], list<Account>& list_accounts, int atm_id)
{
	list_lock.rd_enter();
	list<Account> :: iterator it = find_account(args, list_accounts, atoi(args[1])); //?? value not related in 4 arg
	if(it == list_accounts.end()) // In case the account does not exist
	{
		pthread_mutex_lock(&mutex_logfile);
		logfile << "Error " << atm_id+1 << ": Your transaction failed - account id " << args[1] << " does not exist" << endl;
		pthread_mutex_unlock(&mutex_logfile);
		sleep(1);
	}
	else // In case the account exists
	{
		it->rd_wrt.wrt_enter(); 	       // Lock the account
		if(it->password == atoi(args[2]))  // In case the password is correct
		{
			if(it->amount < atoi(args[3])) // User tries to withdraw more than possible
			{
				pthread_mutex_lock(&mutex_logfile);
				logfile << "Error " << atm_id+1 << ": Your transaction failed - account id " << args[1] << " balance is lower than " << args[3] << endl;
				pthread_mutex_unlock(&mutex_logfile);
			} 
			else // It is possible to withdraw
			{
				it->amount = (it->amount - atoi(args[3]));
				pthread_mutex_lock(&mutex_logfile);
				logfile << atm_id+1 << ": Account "<< args[1] << " new balance is " << it->amount << " after " << args[3] << " $ was withdrew" << endl;	
				pthread_mutex_unlock(&mutex_logfile);
			}
		}
		else // Password was worng
		{
			pthread_mutex_lock(&mutex_logfile);
			logfile << "Error " << atm_id+1 << ": Your transaction failed - password for account id " << args[1] << " is incorrect" << endl;
			pthread_mutex_unlock(&mutex_logfile);
		}	
		sleep(1);
		it->rd_wrt.wrt_leave();
	}
	list_lock.rd_leave();
	return 0;
}

/* This function prints out to log file account's balance.
   Always returns 0 returns 0 */
int balance_inq  (char *args[MAX_ARG], list<Account>& list_accounts, int atm_id)
{
	list_lock.rd_enter();
	list<Account> :: iterator it = find_account (args, list_accounts, atoi(args[1]));	
	if(it !=  list_accounts.end()) //In case the required account was found:
	{
		it->rd_wrt.rd_enter();     // Protect another ATM from performing actions on the same account
		if (it->password == atoi(args[2])) // Correct password
		{
			pthread_mutex_lock(&mutex_logfile);
			logfile << atm_id+1 << ": Account " << args[1] << " balance is " << it->amount << endl;
			pthread_mutex_unlock(&mutex_logfile);
		}
		else // Incorrect password
		{
			pthread_mutex_lock(&mutex_logfile);
			logfile << "Error" << atm_id+1 << ": Your transaction failed - password for account id " << args[1] << " is incorrect" << endl;
			pthread_mutex_unlock(&mutex_logfile);
		}
		sleep(1);
		it->rd_wrt.rd_leave();
	}
	else // In case the required account was not found:
	{
		pthread_mutex_lock(&mutex_logfile);
		logfile << "Error " << atm_id+1 << ": Your transaction failed - account id " << args[1] << " does not exist" << endl;
		pthread_mutex_unlock(&mutex_logfile);
		sleep(1);
	}
	list_lock.rd_leave();
	return 0;
}

/* This function closes an account.
   Always returns 0 returns 0 */
int quit_acc (char *args[MAX_ARG], list<Account>& list_accounts, int atm_id)
{
	list_lock.wrt_enter();
	list<Account> :: iterator it = find_account (args, list_accounts, atoi(args[1]));	
	if(it !=  list_accounts.end()) //In case the required account was found:
	{
		if (it->password == atoi(args[2])) // Correct password
		{
			pthread_mutex_lock(&mutex_logfile);
			logfile << atm_id+1 << ": Account " << args[1] << " is now closed. Balance was " << it->amount << endl;
			pthread_mutex_unlock(&mutex_logfile);
			list_accounts.erase(it);
		}
		else // Incorrect password
		{
			pthread_mutex_lock(&mutex_logfile);
			logfile << atm_id+1 << ": Your transaction failed - password for account id " << args[1] << " is incorrect" << endl;
			pthread_mutex_unlock(&mutex_logfile);
		}
	}
	else // In case the required account was not found:
	{
		pthread_mutex_lock(&mutex_logfile);
		logfile << "Error " << atm_id+1 << ": Your transaction failed - account id " << args[1] << " does not exist" << endl;
		pthread_mutex_unlock(&mutex_logfile);
	}
	list_lock.wrt_leave();
	return 0;
}

/* This function transfer from one account to another.
   Always returns 0 */
int transfer(char *args[MAX_ARG], list<Account>& list_accounts, int atm_id)
{
	list_lock.rd_enter();
	list<Account> :: iterator it_src = find_account(args, list_accounts, atoi(args[1])); //?? value not related in 4 arg
	list<Account> :: iterator it_dst = find_account(args, list_accounts, atoi(args[3])); //?? value not related in 4 arg
	if(it_src == list_accounts.end()) 	   // In case the source account does not exist
	{
		pthread_mutex_lock(&mutex_logfile);
		logfile << "Error " << atm_id+1 << ": Your transaction failed - account id " << args[1] << " does not exist" << endl; // ??? do we need to say witch
		pthread_mutex_unlock(&mutex_logfile);
		sleep(1);
	}
	else if(it_dst == list_accounts.end()) // In case the dest account does not exist
	{
		pthread_mutex_lock(&mutex_logfile);
		logfile << "Error " << atm_id+1 << ": Your transaction failed - account id " << args[3] << " does not exist" << endl; // ??? do we need to say witch
		pthread_mutex_unlock(&mutex_logfile);
		sleep(1);
	}
	else // In case the accounts exist
	{	
		// Lock the lower account number first:
		if(it_src->account < it_dst->account) 		
		{
			it_src->rd_wrt.wrt_enter();
		    it_dst->rd_wrt.wrt_enter();
		}
		else
		{
			it_dst->rd_wrt.wrt_enter();
			it_src->rd_wrt.wrt_enter();
		}
		if(it_src->password == atoi(args[2]))  // In case password is correct
		{
			if(it_src->amount < atoi(args[4])) // Try to transfer more than possible
			{
				pthread_mutex_lock(&mutex_logfile);
				logfile << "Error " << atm_id+1 << ": Your transaction failed - account id " << args[1] << " balance is lower than " << args[4] << endl;
				pthread_mutex_unlock(&mutex_logfile);
			} 
			else // In case tarnsfer is valid
			{
				it_src->amount = (it_src->amount - atoi(args[4]));
				it_dst->amount = (it_dst->amount + atoi(args[4]));
				pthread_mutex_lock(&mutex_logfile);
				logfile << atm_id+1 << ": Transfer " << args[4] << " from account " << args[1] << " to account " << args[3] << " new account balance is " << it_src->amount << " new target account balance is " << it_dst->amount << endl;	
				pthread_mutex_unlock(&mutex_logfile);
			}
		}
		else // Password was worng
		{
			pthread_mutex_lock(&mutex_logfile);
			logfile << "Error " << atm_id+1 << ": Your transaction failed - password for account id " << args[1] << " is incorrect" << endl;
			pthread_mutex_unlock(&mutex_logfile);
		}
		sleep(1);
		it_src->rd_wrt.wrt_leave();
		it_dst->rd_wrt.wrt_leave();
	}
	list_lock.rd_leave();
	return 0;
}


/* This function finds the required account
return the iterator to the element (in case not found, the it =list.end()*/ 
list<Account> :: iterator find_account (char *args[MAX_ARG], list<Account>& list_accounts, int account)
{
	list<Account> :: iterator it = list_accounts.begin();
	for(int i = 0; i < (int)(list_accounts.size()) ; i++) 
	{
		if(it->account == account)
		{
			break;
		}
		it++;
	}
	return it;
}

/* This function compares between elements of the list according to the their account number.
return true if the first account is smaller the the second */ 
bool compare_accounts (Account elem1, Account elem2)
{
	return (elem1.account < elem2.account);
}