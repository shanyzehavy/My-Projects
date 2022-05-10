#include "Account.h"

 Account :: Account(int accountt, int passwordd, int amountt) // Parameter C'tor
 {
 	account = accountt;
 	password = passwordd;
 	amount = amountt;
 	rd_wrt.init();
 }			

 Account :: ~Account()
 {
 	rd_wrt.destroy_read_write();
 }

 void Account :: print_account()
 {
 	cout << "Account " << account << ": Balance - " << amount << " $ , Account Password - " << password << endl;
 }
 