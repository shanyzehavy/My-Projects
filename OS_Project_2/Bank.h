/* main file. This file contains the main function of bank
*******************************************************************/
#ifndef _BANK_H
#define _BANK_H
#include "Account.h"
#include "ATMs.h"
#include <pthread.h>
#include <unistd.h> 
#include <time.h>

// All required includes are in Account.h

using namespace std;


/* Declare functions:
******************************************************************/
int main(int argc, char *argv[]);
void* commisions (void* comm_thrd_args);
bool is_number(char* str);
void* status_func(void* status_thrd_args) ;



#endif