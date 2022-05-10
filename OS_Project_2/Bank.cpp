#include "Bank.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <pthread.h>
#define MAX_LINESIZE 40
#define MAX_ARG 5
#define handle_error_en(en,msg)		do { errno = en;  perror(msg);	 exit(EXIT_FAILURE);} while (0)

using namespace std;

/* Global/ extern/ static variables: */
list<Account> list_accounts; 	// List_accounts is used to save all accounts in bank 
read_write list_lock;
pthread_mutex_t mutex_logfile;
volatile bool flag_run = true;
ofstream logfile ("log.txt");	// Logfile is used to print activities log into	 	


//**************************************************************************************
// function name: main
// Description: ???
//**************************************************************************************
int main(int argc, char *argv[])
{
	/************* Check validation of input **************/
	int num_atm = atoi(argv[1]);		 // Convert first arg to int
	if ( (num_atm != argc-2) || (num_atm == 0) || (is_number(argv[1]) == false) )  // Invalid arguments - not enough ATM files
	{
		cout << "illegal arguments" << endl;
		return 0; 
	}
	Account bank_acc = Account(0, 0000, 0); 	 	 // Create an account for the bank itself
	list_accounts.push_back(bank_acc); 				 // Insert to list
	list_lock.init();
	/************* Create ATMs thread: **************/ 	
	pthread_t *atm_thrd = (pthread_t*)malloc(sizeof(pthread_t)*num_atm);     // This array will contain each thread's id (atm id) - starts from 1
	if (atm_thrd == NULL)
	{
		cout << "Failed to allocate memory" << endl;
		exit (0);
	}
	Atm_thrd_args *thrd_args = (Atm_thrd_args*)malloc(sizeof(Atm_thrd_args)*num_atm);  	// Holds the argument of each thread for pthread_create
	if (thrd_args == NULL)
	{
		cout << "Failed to allocate memory" << endl;
		exit (0);
	}
	/************* Create commision thread: **************/
	pthread_t comm_thrd;  		  	    // Contain threads' id 
	int s = pthread_create(&comm_thrd, NULL, commisions, NULL);
	if (s != 0)
	{
		cout << "Failed to create thread" << endl;
		handle_error_en(s, "pthread_create");
		exit (0);
	}

	/************* Create status thread: **************/
	pthread_t status_thrd;  		    // Contain threads' id
	s = pthread_create(&status_thrd, NULL, status_func, NULL);
		if (s != 0)
	{
		cout << "Failed to create thread" << endl;
		handle_error_en(s, "pthread_create");
		exit (0);
	}

	/************* Init logfile mutex: **************/
	s = pthread_mutex_init(&mutex_logfile, NULL);
	if (s != 0)
	{
		cout << "Failed to create thread" << endl;
		//errno(s, "pthread_mutex_init");
		exit (0);
	} 
	for(int i = 0; i < num_atm; i++) 
	{ 
		//string filename(argv[i+2]);
		thrd_args[i] = Atm_thrd_args(i, argv[i+2]); // Create an arg to send to atm function
		Atm_thrd_args *ptr_args = &(thrd_args[i]);
		s = pthread_create( &atm_thrd[i], NULL, atm, (void*)ptr_args);
		if (s != 0)
		{
			cout << "Failed to create thread" << endl;
			handle_error_en(s,"pthread_create");
			exit (0);
		}
	} 
	for(int i = 0; i < num_atm; i++) 
	{ 
		s = pthread_join(atm_thrd[i], NULL);
			if (s != 0)
		{
			cout << "Failed to create thread" << endl;
			handle_error_en(s,"pthread_join");
			exit (0);
		} 
	}
	flag_run = false;
	s = pthread_join(comm_thrd, nullptr);
	if (s != 0)
	{
		cout << "Failed to create thread" << endl;
		handle_error_en(s,"pthread_join");
		exit (0);
	}
	s = pthread_join(status_thrd, nullptr);
	if (s != 0)
	{
		cout << "Failed to create thread" << endl;
		handle_error_en(s,"pthread_join");
		exit (0);
	}
	s = pthread_mutex_destroy(&mutex_logfile); 
	if (s != 0)
	{
		cout << "Failed to create thread" << endl;
		exit (0);
	}
	list_lock.destroy_read_write(); // Delete the list's lock
	logfile.close();
    return 0;
}


/* This function sets the start routine of the commisions thread */
void* commisions (void* comm_thrd_args)
{
	while(flag_run)
	{
		srand (time(NULL));
		int rand_num = (rand()%3)+2;	// Random (natural) number from 2-4
		list_lock.rd_enter();			// Locks the data structure of the accounts (list)
		list<Account> :: iterator it = list_accounts.begin();
		it++; 							// First item on the list is the bank itself
		for(int i = 0; i < (int)(list_accounts.size())-1; i++) 
		{			
			list_accounts.begin()->rd_wrt.wrt_enter();  	// Lock bank's account as writer - first item on list_accounts
			it->rd_wrt.wrt_enter(); 				    	// Lock account as writer
			double comm = 0.01*rand_num*it->amount;	     	// Calculate each account's commission 
			list_accounts.begin()->amount = (list_accounts.begin()->amount) + (int)comm;
			it->amount = it->amount - (int)comm;		
			pthread_mutex_lock(&mutex_logfile);	
			logfile << "Bank: commissions of " << rand_num << " % were charged, the bank gained " << (int)comm << " $ from account " << it->account << endl;
			pthread_mutex_unlock(&mutex_logfile);
			list_accounts.begin()->rd_wrt.wrt_leave();
			it->rd_wrt.wrt_leave();
			it++;
		}
		list_lock.rd_leave();
		sleep(3);
	}
	pthread_exit(NULL);
}

/* This function gets string from main to check whether input is valid (number of atms)
in case input is valid returns true, otherwise - false*/ 
bool is_number(char* str)
{
	for(int i = 0; i < (int)strlen(str); i++)
	{
		if(isdigit(str[i]) == 0)
			return false;
	}
	return true;
}


/* This function prints the current bank status.
  Always returns 0 */ 
void* status_func(void* status_thrd_args) 
{ 
	while(flag_run)
	{
		list_lock.rd_enter();
		printf("\033[2J");
		printf("\033[1;1H");
		list<Account> :: iterator it = list_accounts.begin();
		it++; 			// The first element of the list is always the bank
		cout << "Current Bank Status" << endl;
		for(int i = 0; i < (int)list_accounts.size()-1 ; i++) 
		{
			it->rd_wrt.rd_enter();
			it->print_account();
			it->rd_wrt.rd_leave();
			it++;
		}
		it = list_accounts.begin(); // Pointing to the bank element
		it->rd_wrt.rd_enter();
		cout << "The Bank has " << it->amount << " $" << endl;
		it->rd_wrt.rd_leave();
		list_lock.rd_leave();
		usleep (500000);
	}
	pthread_exit(NULL);
}








