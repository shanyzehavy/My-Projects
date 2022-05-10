#include "Client.h"

Client :: Client (struct sockaddr_in clnt_addrr, unsigned int clnt_addr_lenn, string file_namee,  FILE* clnt_filee)
{
	memset(&clnt_addr, 0, sizeof(clnt_addr));
	clnt_addr             = clnt_addrr;
	clnt_addr_len         = clnt_addr_lenn;
	block_num 			  = 0;
	fail_num 			  = 0;
	clnt_file			  = clnt_filee;
	tv.tv_sec 			  = 0;
	tv.tv_usec  		  = 0;
	file_name             = file_namee;
}
