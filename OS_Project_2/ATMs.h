/* ATMs file. This file contains the ATMs fucntions
*******************************************************************/
#ifndef _ATMS_H
#define _ATMS_H
#include "Account.h"
#define MAX_LINESIZE 40
#define MAX_ARG 5
#include <fstream>

using namespace std;

/* Declare functions:
******************************************************************/

void *atm 		 (void *ptr_args);
int atm_exec 	 (char *line, list<Account>& list_accounts,int atm_id) ;
int open_acc 	 (char *args[MAX_ARG], list<Account>& list_accounts, int atm_id);
int deposit_acc  (char *args[MAX_ARG], list<Account>& list_accounts, int atm_id);
int withdraw_acc (char *args[MAX_ARG], list<Account>& list_accounts, int atm_id);
int balance_inq  (char *args[MAX_ARG], list<Account>& list_accounts, int atm_id);
int quit_acc     (char *args[MAX_ARG], list<Account>& list_accounts, int atm_id);
int transfer 	 (char *args[MAX_ARG], list<Account>& list_accounts, int atm_id);
bool compare_accounts (Account elem1, Account elem2);
list<Account> :: iterator find_account (char *args[MAX_ARG], list<Account>& list_accounts, int account);
using namespace std;

class Atm_thrd_args
{
	public:

		int atm_id;
		char* file_name;
		Atm_thrd_args(): atm_id(0), file_name(NULL){}  // Default C'tor 
		Atm_thrd_args(int atm_id, char* file_name): atm_id(atm_id), file_name(file_name){} // C'tor with args
};

#endif