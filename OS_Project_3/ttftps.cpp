#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <sys/time.h>
#include <climits>
#include "Client.h"

//#define RESPONSE_MSG "I got your message”
using namespace std;

#define MAX_PAC 516
#define MAX_DATA 512
#define MTU 1500 
#define ECHOMAX 255 /* Longest string to echo */

list<Client> list_clnts;

void error(const char *msg) {
	perror(msg);
	list_clnts.clear();
	exit(1);
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

/* This function finds the required client
return the iterator to the element (in case not found, the it =list.end()*/ 
list<Client> :: iterator find_clnt (struct sockaddr_in addr )
{
	list<Client> :: iterator it = list_clnts.begin();
	for(int i = 0; i < (int)(list_clnts.size()) ; i++) 
	{
		// check those are the correct fields
		if ((it->clnt_addr.sin_family == addr.sin_family) && (it->clnt_addr.sin_addr.s_addr == addr.sin_addr.s_addr) && (it->clnt_addr.sin_port == addr.sin_port))
		{
			break;
		}
		it++;
	}
	return it;
}


/* This function finds the client who's timeout hadexpired
return the iterator to the element (in case not found, the it =list.end()*/ 
list<Client> :: iterator find_timeout (struct timeval timeout , int *sock)
{
	timeval tmp, res;
	Ack_pac ack_pac;
	gettimeofday(&tmp,NULL);

	list<Client> :: iterator it = list_clnts.begin();
	for(int i = 0; i < (int)(list_clnts.size()) ; i++) 
	{
		timersub(&tmp,&it->tv,&res);  // Sub: res=tmp-tv (current time - earliest packet arrival time)
		// check those are the correct fields
		if (timeout.tv_sec < res.tv_sec)
		{
			break;
		}
		else if((timeout.tv_sec == res.tv_sec) && (timeout.tv_usec < res.tv_usec))
		{
			break;
		}
		it++;
	}
	if(it != list_clnts.end())
	{
		// send ack for last message (sendto)
		ack_pac.block  = htons(it->block_num); 
		ack_pac.opcode = htons(4);
		int s = sendto(*sock, &ack_pac, sizeof(ack_pac), 0,(struct sockaddr *) &(it->clnt_addr), sizeof(it->clnt_addr));
		gettimeofday(&(it->tv),NULL); // Start timeout timer of client an ack was sent to
		if (s != sizeof(ack_pac))
		{ 
			error("TTFTP_ERROR");
		}
	}
	return it;
}

/* This function finds the minimal timeout */ 
timeval min_timeout (long timeout)
{
	/**** Create and initialize minimum timeout variable ****/
	timeval min; 
	min.tv_usec = LONG_MAX;
	min.tv_sec  = LONG_MAX;
	list<Client> :: iterator it = list_clnts.begin();
	list<Client> :: iterator tmp_it;
	for(int i = 0; i < (int)(list_clnts.size()) ; i++) 
	{
		// check those are the correct fields
		// FInd client who's timer started earliest
		if (it-> tv.tv_sec < min.tv_sec)
		{
			min.tv_usec =  it-> tv.tv_usec;
			min.tv_sec  = it-> tv.tv_sec;
			tmp_it = it;
		}
		else if((it-> tv.tv_sec == min.tv_sec) && (it->tv.tv_usec < min.tv_usec))
		{
			min.tv_usec =  it-> tv.tv_usec;
			min.tv_sec  = it-> tv.tv_sec;
			tmp_it = it;
		}
		it++;
	}
	// Subtruct current time from the min time that we found (earliest packet arrival): (Need to improve or use designated function)
	timeval tmp, res;
	gettimeofday(&tmp,NULL);
	timersub(&tmp,&min,&res);     // Sub: res=tmp-min (current time - earliest packet arrival time)
	tmp.tv_sec  = timeout;
	tmp.tv_usec = 0;
	timersub(&tmp,&res,&min); // Find remaining time till timeout (insert into min)
	if ((min.tv_sec < 0) || (min.tv_sec < 0))
	{
		min.tv_sec  = 0;
		min.tv_usec = 0;
	}
	return min;
}
void send_error_pac (int *sock, const char* msg, unsigned short error_code, struct sockaddr_in clnt_addr)
{
	struct Error_pac error_pac;
	error_pac.opcode = htons(5);
	error_pac.error = htons(error_code);
	strcpy(error_pac.msg, msg);
 	int s = sendto(*sock, &error_pac, sizeof(error_pac), 0,(struct sockaddr *) &(clnt_addr), sizeof(clnt_addr));
	if (s != sizeof(error_pac))
	{ // In case sendto failed
		error("TTFTP_ERROR");
	}
}


void bail_out_handler (int *clnt_cntr, list<Client> :: iterator it, const char* msg ) 
{
	// Fatal error bail out for client pointed by it
	(*clnt_cntr)--; 
	int cls = fclose (it->clnt_file);
	if (cls != 0) 
		error("TTFTP_ERROR");
	int r = remove(it->file_name.c_str()); // Delete the file
	if (r != 0)
	{
		error("TTFTP_ERROR"); //check
	}
	list_clnts.erase(it); // Remove client from list
}

/*This function opens a file and sends first ack once a new request arrives
  on succes return the client element*/
Client* wreq_handler(Pac tmp_pac, int *sock, struct sockaddr_in clnt_addr)
{
	Ack_pac ack_pac;
	Wreq_pac wreq_pac;
	wreq_pac.opcode = 2;
	memcpy(wreq_pac.wreq, &tmp_pac.first_pad, sizeof(wreq_pac.wreq));
	int i=0;
	if(wreq_pac.wreq[0]=='\0') 
	{
		send_error_pac (sock,"Illegal WRQ",4, clnt_addr);
		return NULL;
	}
	// Look for first '\0' (after filename)
	while ((wreq_pac.wreq[i] != '\0') && (i < (int) sizeof(wreq_pac.wreq)))
	{
		i++;
	}
	//now i is on \0
	if  (i == sizeof(wreq_pac.wreq))  // no \0 after filename
	{	
		send_error_pac (sock,"Illegal WRQ",4, clnt_addr);
		return NULL;
	}
	if( i+6 >= (int) sizeof(wreq_pac.wreq))	
	{
		send_error_pac (sock,"Illegal WRQ",4, clnt_addr);
		return NULL;
	}
	// Look for second \0 (after file mode)
	if(wreq_pac.wreq[i+1] == '\0')
	{
	    send_error_pac (sock,"Illegal WRQ",4, clnt_addr);
	    return NULL;
	}
	i++;
	while ((wreq_pac.wreq[i] != '\0') && (i < (int) sizeof(wreq_pac.wreq)))
	{
		i++;
	}
	if  (i == sizeof(wreq_pac.wreq))  // no \0 after filename
	{	
		send_error_pac (sock,"Illegal WRQ",4, clnt_addr);
		return NULL;
	}
	
	char filename [MAX_PAC-OPC_LEN]; //whtas the biggest file name? 
	char mode_field[MTU];
	//if(strlen(wreq_pac.wreq())) check????? in case we need to check file name max sixe is 255
	strcpy(filename, wreq_pac.wreq);
	if(strlen(filename) == 0) // In case the file name is not valid
	{
		send_error_pac (sock,"Illegal WRQ",4, clnt_addr);
		return NULL;
	}
  	if(strchr(filename, (int)'/') != NULL)  // In case the file name is not valid
	{
		send_error_pac (sock,"Illegal WRQ",4, clnt_addr);
		return NULL;
	}
	strcpy(mode_field,((wreq_pac.wreq)+strlen(filename)+1));
	if(strcmp(mode_field, "octet") != 0)
	{
		send_error_pac (sock,"Illegal WRQ",4, clnt_addr);
		return NULL;
	}
	// Create the file
	FILE* clnt_file;
	if(fopen(filename,"r")) // file exists
	{
		send_error_pac (sock,"File already exists",6, clnt_addr);
		return NULL;
	}
	clnt_file = fopen(filename,"w+"); 
	if(clnt_file == NULL) 
	{
		error("TTFTP_ERROR");
	}

	Client *ptr_clnt = new Client (clnt_addr, sizeof(clnt_addr), filename,  clnt_file);
	/* Send ack0 back to the client */
	ack_pac.block  = htons(0);
	ack_pac.opcode = htons(4);
	int s = sendto(*sock, &ack_pac, sizeof(ack_pac), 0,(struct sockaddr *) &clnt_addr, sizeof(clnt_addr));
	gettimeofday(&(ptr_clnt->tv),NULL); // Start timeout timer of client an ack was sent to
	if (s != sizeof(ack_pac)){         // In case sendto failed
		error("TTFTP_ERROR");
	}
	return ptr_clnt;
}

int main(int argc, char *argv[]) {
	/**** Arguments validation:****/
	if(argc != 4) { 
		cout << "TTFTP_ERROR: illegal arguments" << endl;
		return 0;
	}
	// check validation of input arguments (only digits)
	for (int i=1; i<4; i++) { 	
		if (is_number(argv[i]) != true) {
			cout << "TTFTP_ERROR: illegal arguments" << endl;
			return 0;
		}
		if ((atoi(argv[i]) < 1)|| (atoi(argv[i]) > USHRT_MAX)) {
			cout << "TTFTP_ERROR: illegal arguments" << endl;
			return 0;
		}
	}
	// Check validation - portnum:
	if ((atoi(argv[1]) < 1024) || (atoi(argv[1]) > 65535)) {
		cout << "TTFTP_ERROR: illegal arguments" << endl;
		return 0;
	}
	// Check validation of timeout and fail_num:
	if ((atoi(argv[2]) < 0) || (atoi(argv[3])<0)) {
		cout << "TTFTP_ERROR: illegal arguments" << endl;
		return 0;
	}
	int sock;					   	  /* Socket */
	struct sockaddr_in serv_addr;     /* Local address */
	struct sockaddr_in clnt_addr;	  /* Client address */
	unsigned int cli_addr_len;	 	  /* Length of incoming message */
	int recv_msg_size;			   	  /* Size of received message */
	struct Wreq_pac wreq_pac;	
	struct Ack_pac ack_pac;      
	unsigned short serv_port = atoi(argv[1]);	 /* Server port */
	long timeout  = (long)atoi(argv[2]);
	int max_fail = atoi(argv[3]);	  /* Input for maximum amount of failures per client */
	struct timeval tv;
	tv.tv_sec  = timeout;
	tv.tv_usec = 0;
	int clnt_counter = 0;
	list<Client> :: iterator tmp_it;
	/* Create socket for sending/receiving datagrams */
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		error("TTFTP_ERROR");
	}
	/* Construct local address structure */
	/* Zero out structure */
	memset(&serv_addr, 0, sizeof(serv_addr));
	memset(&clnt_addr, 0, sizeof(clnt_addr));
	/* Internet address family */
	serv_addr.sin_family = AF_INET;
	/* Any incoming interface */
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	/* Local port */
	serv_addr.sin_port = htons(serv_port);
	/* Bind to the local address */
	if (bind(sock, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)	{
		error("TTFTP_ERROR"); 
	}
	for (;;) { /* Run forever */
		fd_set read_fd;
		int lst_wrt_size = 0;  // Used to indicate last msg (<512)
		struct Pac tmp_pac;	   // Insert pac into the temporary Pac object
		struct Data_pac data_pac;
		/* INitialize the packets: */
		memset(&tmp_pac, '\0', sizeof(tmp_pac));
		memset(&data_pac, '\0', sizeof(data_pac));
		/* Set the size of the in-out parameter */
		cli_addr_len = sizeof(clnt_addr);
		/* Block until receive message from a client */
		recv_msg_size = recvfrom(sock, (void*)&tmp_pac, sizeof(wreq_pac), 0,(struct sockaddr *) &clnt_addr, &cli_addr_len);
		if(recv_msg_size < 4)
		{
			send_error_pac (&sock,"Illegal TFTP operation",4,clnt_addr); // Not a logical size of pakcet to recieve
			continue;	
		}
		/** This pakcet must be wreq packet, therefore we hanlde it as one: **/
		tmp_pac.opcode = ntohs(tmp_pac.opcode);
		if ( recv_msg_size < 0) {
			error("TTFTP_ERROR");
		}
		if (recv_msg_size == 0) {
			continue;
		}
		// in case of invalid request (opcode) - move on to next process
		if ((tmp_pac.opcode < 2) || (tmp_pac.opcode > 4 )) // In case the opcode is not valid:
		{
			send_error_pac (&sock,"Illegal TFTP operation",4,clnt_addr);
			continue;
		}
		if (tmp_pac.opcode != 2){ 
			send_error_pac (&sock, "Unknown user",7 , clnt_addr);
			continue;
		}
		Client* clnt = wreq_handler(tmp_pac, &sock, clnt_addr); // Open a file and send ack
		if(clnt == NULL)
		{
			continue; // In case the request is invalid - bail out
		}
		list_clnts.push_back(*clnt);
		delete clnt;
		clnt_counter++;
		// algorithm
		bool bail_out = false;
		int sel;
		list<Client> :: iterator it;
		do
		{
			do
			{
				do 
				{
					bail_out = false;
					memset(&tmp_pac, '\0', sizeof(tmp_pac));
					memset(&data_pac, '\0', sizeof(data_pac));
					tmp_it =  find_timeout(tv, &sock);
					while(tmp_it != list_clnts.end())
					{
						tmp_it->fail_num++; // Increase failuer counter after timeour accoured
						if (max_fail < tmp_it->fail_num) // In case fail_num is larger than max fail num
						{
							// Fatal error bail out for client pointed by it
							bail_out = true;
							send_error_pac (&sock,"Abandoning file transmission",0, clnt_addr);
							bail_out_handler(&clnt_counter, tmp_it, "Abandoning file transmission");
						}
						tmp_it = find_timeout(tv, &sock);
					}
					if(clnt_counter == 0)
						break;


					// Wait for timeout (for a signle package)
					FD_ZERO (&read_fd);
					FD_CLR (sock, &read_fd);
					FD_SET (sock, &read_fd);
					/** Create timeout to send **/
					struct timeval min = min_timeout(timeout);
					sel = select (sock+1 , &read_fd, NULL, NULL, &min); /* sock + 1 max fd ready */
					if (sel < 0) // Select failed:
					{
						list_clnts.clear();
						error("TTFTP_ERROR");
					}
					if (sel > 0) // not timeout
					{
						// read data from socket
						recv_msg_size = recvfrom(sock, (void*)&tmp_pac, sizeof(wreq_pac), 0,(struct sockaddr *) &clnt_addr, &cli_addr_len);
						tmp_pac.opcode = ntohs(tmp_pac.opcode);		
						if(recv_msg_size < 4)
						{
							bail_out = true;
							tmp_it = find_clnt(clnt_addr);
							send_error_pac (&sock,"Illegal TFTP operation",4,clnt_addr);
							if (tmp_it != list_clnts.end()) // In case client exists - use bailout handler to earase it
							{
								bail_out_handler(&clnt_counter, tmp_it, "Illegal TFTP operation");
								if(clnt_counter == 0)
									break;	
							}
						}
					}
					else if (sel == 0) // timeout expired
					{
						// How to understand which client's timeout was expired?
						it = find_timeout(tv, &sock); 
						it->fail_num++; // Increase failuer counter after timeour accoured
						if (max_fail < it->fail_num) // In case fail_num is larger than max fail num
						{
							// Fatal error bail out for client pointed by it
							bail_out = true;
							send_error_pac (&sock,"Abandoning file transmission",0, clnt_addr);
							bail_out_handler(&clnt_counter, it, "Abandoning file transmission");
		
							if(clnt_counter == 0)
								break;
						}
					}
				} while (sel == 0); // While we didnt get anything
				if(clnt_counter == 0 || bail_out == true) { 
					break;
				}
				if ((tmp_pac.opcode < 2) || (tmp_pac.opcode > 4 )) // In case the opcode is not valid:
				{
					bail_out = true;
					tmp_it = find_clnt(clnt_addr);
					send_error_pac (&sock,"Illegal TFTP operation",4,clnt_addr);
					if (tmp_it != list_clnts.end()) // In case client exists - use bailout handler to earase it
					{
						bail_out_handler(&clnt_counter, tmp_it, "Illegal TFTP operation");
						if(clnt_counter == 0)
							break;	
					}	
				}
				else if (tmp_pac.opcode == 2 ) // In case its a WRQ packet (opcode = 2):
				{
					it = find_clnt(clnt_addr);
					if(it == list_clnts.end()) // In case the client dosen't exist yet
					{
						Client* client = wreq_handler(tmp_pac, &sock, clnt_addr); // Open a file and send ack0
						if(clnt == NULL)
						{
							bail_out = true;
							continue; // In case the request is invalid - bail out
						}
						// In case client is valid - insert into list and update counter:
						clnt_counter++;
						list_clnts.push_back(*client);
						//delete client;
						it = find_clnt(clnt_addr);
						bail_out = true; // Not an actual bailout but continue to next iteration
					}
					else // Client with the same clnt_addr alraedy exists
					{
						bail_out = true;
						send_error_pac (&sock, "Unexpected packet",4, clnt_addr);
						bail_out_handler(&clnt_counter, it, "Unexpected packet 4");
						if(clnt_counter == 0)
							break;
					}
				}
				else if (tmp_pac.opcode == 3) // In case the packet is a data packet (opcode == 3):
				{
					it = find_clnt(clnt_addr);
					if(it == list_clnts.end()) // In case the client dosen't exist yet
					{
						send_error_pac (&sock, "Unknown user",7 , clnt_addr);
						bail_out = true;
					}
					else // Client with clnt_addr exists
					{
						it->fail_num = 0; // Reset counter of failures in cas of succseful msg
						/** Convert tmp into data pac **/
						memcpy(&data_pac.block, &tmp_pac.first_pad,sizeof(unsigned short));
						memcpy(data_pac.data, tmp_pac.pac, sizeof(data_pac.data));
						/** Compare block num with prv block num **/
						unsigned short next_block_num =  it->block_num +1; 
						if (next_block_num != ntohs(data_pac.block)) // In case the data pac contains wrong number of block
						{
							bail_out = true;
							send_error_pac (&sock,"Bad block number",0, clnt_addr);
							bail_out_handler (&clnt_counter, it,"Bad block number 0");	
							if(clnt_counter == 0)
								break;					
						}
						else // In case the block num is as expected
						{
							strcpy(it->data, data_pac.data);
						}
					}
				}
			} while (false);
			if (clnt_counter == 0) 	// 	In case there are no clients - go to the for loop
				break;
			if (bail_out == true) // In case of bailout jump to the first dowhile iteration
				continue;
			// to_exp_counter = 0;
			lst_wrt_size = fwrite ((void*)(it->data),sizeof(char), recv_msg_size-4, it->clnt_file); // Write next bulk of data	
			it->block_num++;
			// Send ack packet to client
			ack_pac.block  = data_pac.block; // The block number is getting from the client, no need in htons()
			ack_pac.opcode = htons(4);
			int s = sendto(sock, &ack_pac, sizeof(ack_pac), 0,(struct sockaddr *) &(it->clnt_addr), sizeof(clnt_addr));
			gettimeofday(&(it->tv),NULL); // Start timeout timer of client an ack was sent to
			if (s != sizeof(ack_pac))
			{ 
				error("TTFTP_ERROR");
			}
			if(lst_wrt_size < MAX_DATA) // Last msg from client
			{
				clnt_counter--;
				int cls = fclose (it->clnt_file); 
				if (cls != 0) 
					error("TTFTP_ERROR");
				list_clnts.erase(it); // Remove client from list	
			}
		} while (clnt_counter > 0); // Still have more blocks from client (not last packet)
	}
	/* NOT REACHED */
}




