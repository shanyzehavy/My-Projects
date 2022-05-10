#ifndef _JOB_H
#define _JOB_H
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdbool>
#include <iostream>
#include <list>
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
//typedef enum { FALSE , TRUE } bool;


using namespace std;
extern size_t cmd_ser_num;

 class job {
 	public:
 		string proc_name;
 		size_t pid;
 		//size_t ser_num = cmd_ser_num++;
 		size_t ser_num;
 		size_t begin_time;
 		bool stopped = false;

 		job();
 		job(string proc_name, size_t pid, size_t ser_num, size_t begin_time, bool stopped);
 		job(const job &j1);
 		void print_job();

  };

#endif