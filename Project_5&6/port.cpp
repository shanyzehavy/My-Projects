#include "port.h"
#include "string.h"

using namespace std;

enum {FROM = 0, TO =1 };

Port::Port(String pattern): Field(pattern), from(0), to(0)  {};

Port::~Port(){ }

bool Port::match_value(String val) const {

	// Convert a given string of port number to int:
	unsigned short numeric_val = (unsigned short)val.to_integer(); 
	return ((numeric_val >= (from)) && (numeric_val <= (to) ));
} 

bool Port::set_value(String val) {
	
	parse(val); // Parse sets the FROM and TO members accordingly
	return true;
}


void Port::parse(String val){

	String *array; 	
	size_t size = 0;
	// Split given string according to delimiters:
	// Size is always 2 according to our implementation:
	val.split("- ,", &array, &size);  
	// Save FROM and TO as members:
	from = array[FROM].to_integer();
	to = array[TO].to_integer();

	delete[] array; // The memory was allocated in split func
}
