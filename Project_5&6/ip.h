#ifndef IP_H
#define IP_H

#include "field.h"
#include "string.h"
#include <cmath>



class Ip : public Field {

	protected:

		unsigned int from;
		unsigned int to;

	    /**
	     * @brief Compares given IP value with the condition
	     * @param argc String object for condition value (before parsing)
	     * @param argv 1 given arguments
	     * @return True in case IP value is valid accroding to conditions
	     */
	    bool match_value(String val) const;

	public:

		/**
	     * @brief Inititate an IP object with given patern (src/ dest IP)
	     */
		Ip(String pattern);


	    
	    /**
	     * @brief Destory an IP object
	     */
	    ~Ip();

		/**
	     * @brief Sets range values for ip condition (given from main)
	     * @param argc String object for condition value (before parsing)
	     * @param argv 1 given arguments: String
	     * @return True in case of succeful parsing
	     * @notes Updates the value of members 'from' and 'to' 
	     * acording to parsing checks and mask
	     */
	    bool set_value(String val);



	    /**
	     * @brief Parses given String value into either IP or IP conditions
	     * @param argc String object and pointer to integer 
	     * @param argv 2 given arguments
	     * @notes In case of a mask within val -  mask value is updated
	     */
	    unsigned int parse(String val, int* mask) const;




	    
};

#endif
