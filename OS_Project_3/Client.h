/* Client class, Data pack struct, Ack pack struct, WREQ pack struct
*******************************************************************/
#ifndef _CLIENT_H
#define _CLIENT_H
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
#include <istream>
#include <ostream>
#include <fstream>
#include <queue>
#include <list>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
using namespace std;

#define MAX_PAC 516
#define MAX_DATA 512
#define MTU 1500 
#define OPC_LEN sizeof(unsigned short)
#define ECHOMAX 255 /* Longest string to echo */

struct Pac {
	unsigned short opcode;
	char 		   first_pad;
	char 		   sec_pad;
	//char 		   align_for_us = '\0';          
	char 		   pac[MTU-2*OPC_LEN];
}__attribute__((packed));

struct Wreq_pac {
	unsigned short opcode;
	char           wreq [MTU-OPC_LEN]; // file name, octet
}__attribute__((packed));

struct Data_pac {
	unsigned short opcode;
	unsigned short block;
	char           data [MAX_DATA];
}__attribute__((packed));

struct Ack_pac {
	unsigned short opcode;
	unsigned short block;
}__attribute__((packed));

struct Error_pac {
	unsigned short opcode;
	unsigned short error;
	char           msg[MAX_DATA];
}__attribute__((packed));

class Client {
 	public:
 		/**************   Declare members:   **************/
 		struct       sockaddr_in clnt_addr;
 		unsigned int   clnt_addr_len;
 		string         file_name;
 		FILE* 		   clnt_file;
 		int            block_num;
 		int            fail_num;
 		char 		   data [MAX_DATA];
 		struct timeval tv;
 		/**************   Declare functions:   **************/
 		Client (struct sockaddr_in clnt_addrr, unsigned int clnt_addr_lenn, string file_namee, FILE* clnt_filee);
	};

	#endif