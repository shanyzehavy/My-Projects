/* Account class
*******************************************************************/
#ifndef _ACCOUNT_H
#define _ACCOUNT_H
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cstdio>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <cstdbool>
#include <iostream>
#include <fstream>
#include <queue>
#include <list>
#include <semaphore.h>
#include <pthread.h>
#include <mutex>
using namespace std;

struct read_write {

	int counter_rd;
	sem_t rd_lock;
	sem_t wrt_lock;
	void init () 
	{
		counter_rd = 0;
		sem_init(&rd_lock, 0, 1);
		sem_init(&wrt_lock, 0, 1);
	}

	void rd_enter ()
	{
		sem_wait(&rd_lock);
		counter_rd++;
		if(counter_rd == 1) {
			sem_wait(&wrt_lock);
		}
		sem_post(&rd_lock);
	}

	void wrt_enter ()
	{
		sem_wait(&wrt_lock);
	}

	void rd_leave ()
	{
		sem_wait(&rd_lock);
		counter_rd--;
		if(counter_rd == 0)
			sem_post(&wrt_lock);
		sem_post(&rd_lock);
	}

	void wrt_leave ()
	{
		sem_post(&wrt_lock);
	}

	void destroy_read_write()
	{
	 	sem_destroy(&rd_lock);
	 	sem_destroy(&wrt_lock);
	}
};

class Account {
 	public:
 		// Declare members:
 		int account;
 		int password;
 		int amount;
 		read_write rd_wrt;

 		Account(int accountt, int passwordd, int amountt);
 		~Account();
 		void print_account();

  };

#endif