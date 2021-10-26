#include "string.h"
#include "field.h"
#include "ip.h"
#include "port.h"
#include "input.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>


enum {COND_NAME = 0, COND_NUM = 1 };

using namespace std;


/**
 * @brief Initiate a Field objects and use parse function 
 * @param argc 1 arguments given
 * @param argv a path to a file
 * @return return 0
 */
int main(int argc, char **argv){
	
	int valid_args = check_args(argc, argv); // Check valid input
	if (valid_args != 0) {

		return 1;  
	}
	
	// Create a new string object with given condition:
	String condition (argv[1]); 
	String *array; 				
	size_t size = 0;
	condition.split("=, ", &array, &size);
	Field *ptr_field;

	if ((array[COND_NAME].equals("src-ip")) || 
		(array[COND_NAME].equals("dst-ip"))) {
		// Actual type of *ptr_field is ip:
		ptr_field = new Ip (array[COND_NAME]);
	}
	else if ((array[COND_NAME].equals("src-port")) ||
			 (array[COND_NAME].equals("dst-port"))) {
		// Actual type of *ptr_field is port:
		ptr_field = new Port (array[COND_NAME]);
	}
	else {
		delete[] array; // The memory was allocated in split func
		return 1; // Should not get here
	}

	ptr_field->set_value(array[COND_NUM]); 

	parse_input(*ptr_field);

	delete[] array; // The memory was allocated in split func
	delete ptr_field;
	return 0;
}