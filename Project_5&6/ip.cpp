#include "ip.h"
#include "string.h"
#define SUB_STRING_NUM 5
#define MAX_MASK 32
#define MIN_MASK 0
#define BASE 2

using namespace std;


enum {FIRST = 0, SECOND =1 , THIRD = 2, FOURTH = 3, MASK = 4};
enum {FIRST_SHIFT = 24, SECOND_SHIFT =16 , THIRD_SHIFT = 8};


Ip::Ip(String pattern): Field(pattern), from(0), to(0)  {};


Ip::~Ip(){ }


bool Ip::set_value(String val){

	int mask = 0; // Mask changes in fucntion val.parse according to given mask
	unsigned int cond_val = parse(val, &mask);
	unsigned int tmp_from  = 0, tmp_to = 0; // Create tmp for bit manipulations
	
	if (mask > MAX_MASK) {
		return false;
	}

	if(mask == MAX_MASK) { // In such case - refer all 32 bits as a condition
		from = cond_val; 
		to = cond_val; 
		return true;
	}
	if(mask == MIN_MASK) { // In such case - no condition
		from = 0; 
		to = ~0; 
		return true;
	}

	tmp_from = pow(BASE,MAX_MASK-mask); 
	tmp_from  = ~tmp_from;
	tmp_from++; // Used for bit manipulations
	tmp_to = ~tmp_from; // Used for bit manipulations

	from = (cond_val)&(tmp_from); // From value for valid IP **
	to = (cond_val)|(tmp_to); // To value for valid IP **

	return true; 
}


bool Ip::match_value(String val) const{

	int mask = 0; // Mask is not used in here
	unsigned int numeric_val = parse(val, &mask); 
	return ((numeric_val >= (from)) && (numeric_val <= (to) ));
}


unsigned int Ip::parse(String val, int *mask) const{

	String *array; 
	size_t size = 0; 
	
	// Split given string according to delimiters:
	val.split("./ ,", &array, &size); 
	// Create an array with 4 strings and mask value:
	int arr_int[(int)size] = {0}; 
	for(int i = 0 ; i < (int)size; i++){
		arr_int[i] = array[i].to_integer(); // Save each string as an integer
	}
	if((int)size == MASK+1){ // In case of 5 elements - the fifth is the mask
		*mask = arr_int[MASK];
	}
	// Create an integer with binary manipulations:
	unsigned int ret_val = (arr_int[FIRST] << FIRST_SHIFT | 
		   					arr_int[SECOND] << SECOND_SHIFT | 
		   					arr_int[THIRD] << THIRD_SHIFT| arr_int[FOURTH]); 

	delete[] array; // The memory was allocated in split func

	return ret_val;

}


