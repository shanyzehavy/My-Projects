#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <queue>
#include <list>
#include "job.h"

void sig_tstp(int sig);
void sig_init(int sig);


#endif

