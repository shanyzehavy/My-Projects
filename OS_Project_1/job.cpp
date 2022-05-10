#include "job.h"

 job :: job() : proc_name(""), pid(0), begin_time(0), stopped(false) {} //C'tor
 job :: job(string proc_name, size_t pid, size_t ser_num, size_t begin_time, bool stopped) : 
 proc_name(proc_name), pid(pid), ser_num(ser_num), begin_time(begin_time), stopped(stopped) {} //C'tor with arguments
 job ::job(const job &j1)
 {
 	proc_name = j1.proc_name;
 	pid = j1.pid;
 	ser_num = j1.ser_num;
 	begin_time = j1.begin_time;
 	stopped = j1.stopped;
 }

 void job :: print_job(){
 	cout << "[" << ser_num << "] " << proc_name << " : " << pid << " " << difftime(time(NULL),begin_time) << " secs";
 	if(stopped == true)
 	{
 		cout << " (stopped)";
 	}
 	cout << endl;
 }
