#include "String.h"
#pragma warning(disable: 4996) //warning C4996: 'fopen' was declared deprecated
#pragma warning(disable: 4267) //warning C4267: 'initializing' : conversion from 'size_t' to 'int', possible loss of data

char *str (
	char *string,
 	const char *format, 
	... 
){
	static char BUFFER[1024];
	if (string==0)string = BUFFER;
  
	va_list args ;
    va_start( args, format ) ; 
    vsprintf (string,format, args); 
	return string;
}  