
#include "string.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#define MIN_DIG '0'
#define MAX_DIG '9'
#define END_OF_STR '\0'


using namespace std;


String::String(){

    data = new char[END_OF_STR];
    length = 0;
}


String::String(const String &str){

    length = str.length;
    data = new char[length+1];
    strcpy(data, str.data);
}


String::String(const char *str){

    // In case the pointer is null, allocate an empty string:
    if(str == NULL){
        data = new char(END_OF_STR); 
        length = 0;
        return;
    }

    length = strlen(str);
    data = new char[length+1];
    strcpy(data, str);
}

String::~String(){

    delete[] data;
}


String& String::operator=(const String &rhs){

    if(this == &rhs){
        return *this;
    }
    delete[] this->data;
    length=rhs.length;
    data= new char[length+1];
    strcpy(data,rhs.data);

    return *this;
}


String& String::operator=(const char *str){

    delete[] data;
    length = strlen(str);
    data = new char[length+1];
    strcpy(data, str);
    return *this;
}


bool String::equals(const String &rhs) const{

    return !(strcmp(data,rhs.data));
}


bool String::equals(const char *rhs) const{

    return !strcmp(data,rhs);
}


void String::split(const char *delimiters, String **output, size_t *size) const{

    int count = 0;
    char *token;
    char *data_cpy= new char[length+1]; // A copy used for the second loop
    strcpy(data_cpy,data);   

    token = strtok(data, delimiters); // Find the first sub-string

    // Count the number of sub-string:
    while(token != NULL){
        count++;
        token = strtok(NULL, delimiters);

    }

    (*size) = count; // Number of sub-string
    if(output == NULL){ 
        delete[] data_cpy;
        return;
    }

    *output = new String[count]; // Allocate an array for the sub-string
    //Fill the array acording to original string:
    token = strtok(data_cpy, delimiters); // Find the first sub-string
    int j = 0;                            // Used for the array index
    while(token != NULL){

        (*output)[j] = String(token); // Allocate memory for each sub-string
        token = strtok(NULL, delimiters);
        j++;
        
    }

   delete[] data_cpy;
}


int String::to_integer() const{

    if(data == NULL) {
        return 0;
    }
    //  Check the string only includes digits for atoi:
    for (int i=0; i<(int)length; i++) {
        if ((data[i]>MAX_DIG)||(data[i]<MIN_DIG)) {
            return 0;
        }
    }
    return atoi(data);
}


String String::trim() const {

    int start = 0, end = length; 
    // Count the number of whitespaces before and after a word:
    while(isspace(*(data+start))){
        start++;
    }
    while(isspace(*(data+end))){
        end--;
    }

    // Allocate memory and create the new string without spaces:
    char *tmp = new char[end-start+1];
    data[end] = END_OF_STR;
    char *str_check = data + start; 
    memcpy(tmp,str_check, end-start+1);
    String str(tmp);
    delete[] tmp;
    return str;
}
