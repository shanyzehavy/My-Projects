#include "field.h"
#include "string.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdbool>
#include <iostream>
#define LOOP_JMP 2
#define FIELD_VAL_JMP 1

using namespace std;

Field::Field(String pattern){

	// Pattern is the name of the condition, which is set numericaly in
	// 'set value':
	this->pattern = pattern; 
}


Field::~Field(){}


bool Field::match(String packet){

	String *array;
	size_t size = 0;
	bool match;
	// Create an array with splitted fields of packet:
	packet.split( "=, ", &array ,&size ); 
	if (size == 0) {
		delete[] array;
		return false; 
	}
	// Going over the output array: compare names of fields with pattern:
	for(int i=0; i<(int)size; i+=LOOP_JMP) {
		array[i] = array[i].trim();
		if(pattern.equals(array[i])) {
			// Check if the numeric value is valid using match_value:
			match = match_value(array[i+FIELD_VAL_JMP]); 

			break;
		}

	}

	delete[] array; // The memory was allocated in split func
	return match;
}



