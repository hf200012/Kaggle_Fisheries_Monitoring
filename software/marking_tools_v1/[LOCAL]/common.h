#pragma once  
// suppress warning:
//   4100; 4512; 4996; 4305; 4127; 4244; 4267; 4701; 4189; 4702;
 
//--------------------------------------------------------------------------- 
// openCV 
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>
 
 
 
//--------------------------------------------------------------------------- 
// Other standard C/C++ libraries  
#include <time.h>
#include <string.h>
#include <fstream>
#include <iomanip>
#include <vector>
#include <float.h>  
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <direct.h>  
#include <errno.h>  
#include <stdarg.h>


// SSE intrinstics functions 
#include <xmmintrin.h>
#include <emmintrin.h>

// openMP functions 
#include <omp.h>

//--------------------------------------------------------------------------- 
// Our libaries 
#include "../[COMMON]/Others/Error.h"
#include "../[COMMON]/Others/String.h"
#include "../[COMMON]/Others/File.h" 
 
 


//--------------------------------------------------------------------------- 
//constant definitions 
#define INF FLT_MAX 
const float PI = 3.141592654f;

#include "defintion.h" 
 

 

 