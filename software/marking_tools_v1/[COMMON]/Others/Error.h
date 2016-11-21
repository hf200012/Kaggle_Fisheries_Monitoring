#pragma once  
#include <string.h>
#include <stdarg.h>
#include <stdio.h >
#include <stdlib.h>
 
//https://msdn.microsoft.com/en-us/library/ms177415%28v=vs.80%29.aspx
//#define ASSERT( z    )   if((z)==0)  printf("error @%s() line %d!\n", __FUNCTION__,__LINE__), exit(0);   
#define ASSERT( z, s ) if((z)==0){ printf("error @%s()! %s\n", __FUNCTION__,s); exit(0); } 

 