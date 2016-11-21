#pragma once  
#include <stdio.h>
#include <direct.h>
#include <io.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <stdarg.h>
#include <io.h>


//printf 
void printf_fprintf( 
	char *filename, 
	char *format, 
	... 
);
 

//file
void fsync(
	FILE *fr
);

 
 
FILE *fopens( 
	char *path,
	char *mode 
);

 
//directory
void mkdirs(
	char *path 
);

 
int count_lines(
	char *list
);
 
char *fgetline( 
	FILE *fr, 
	int is_newline
);

