#ifndef PORT_H
#define PORT_H

#include "field.h"
#include "string.h"
#include <cmath>


class Port : public Field {

	protected:
		unsigned short from;
		unsigned short to;
    	/**
	     * @brief Compares given port value with the condition
	     * @param argc String object for condition value (before parsing)
	     * @param argv 1 given arguments
	     * @return True in case port value is valid accroding to conditions
	     */
	    bool match_value(String packet) const;
	public:
		/**
	     * @brief Inititate an PORT object with given patern (src/ dest PORT)
	     */
	    Port(String pattern);

	    /**
	     * @brief Destory an PORT object
	     */
	    ~Port();

		/**
	     * @brief Sets range values for port condition (given from main)
	     * @param argc String object for condition value (before parsing)
	     * @param argv 1 given arguments: String
	     * @return True in case of succeful parsing
	     * @notes Updates the value of members 'from' and 'to' 
	     * acording to parsing checks and mask
	     */
	    bool set_value(String val);

	    /**
	     * @brief Parses given String value into PORT range
	     * @param argc String object
	     * @param argv 1 given arguments
	     */
	    void parse(String val);
};

#endif
