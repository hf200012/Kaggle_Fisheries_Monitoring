#include "File.h"
#pragma warning(disable: 4267) //warning C4267: 'initializing' : conversion from 'size_t' to 'int', possible loss of data
#pragma warning(disable: 4786) //identifier was truncated to 255 characters in the debug information
#pragma warning(disable: 4996) //warning C4996: 'fopen' was declared deprecated

// BUG: C4786 or C1067 Caused by Long Identifier and /Zi
// Microsoft Knowledge Base Article - 122539 

const char LF = 0x0a; //Line Feed(10) '\n'           
const char CR = 0x0d; //Carriage Return  (13) '\r'
const char COMMENT_CHAR = '#'; 
 
void skip_rest_of_line(
	FILE *fr
);
void skip_comments(
	FILE *fr
);
void skip_whitespace(
	FILE *fr
);

 
// printf   ************************************************************* 

void printf_fprintf( 
	char *filename, 
	char *format, 
	... 
){ 
    va_list arg_ptr ;
    va_start( arg_ptr, format ) ; 
	char line[1024];

	// remove "\r"
	strcpy(line,format);
	char *found=strstr(line,"\r"); 
	while(found!=0){
		found[0]='\n';
		found=strstr(line,"\r");
	}
	 
	FILE* fw=fopens(filename,"a");
	vfprintf(fw,line,arg_ptr) ;
	fclose(fw); 
	vprintf(format,arg_ptr) ;
    va_end(arg_ptr);
}
 
// file ************************************************************ 
  
//fsyn() function:
// reference: http://www.mrx.net/c/introduction.html

void skip_whitespace(
	FILE *fr
){ 
    int ch; 
	do { 
        ch=fgetc( fr ); 
    } while( ch==' ' || ch=='\t' || ch==CR || ch==LF );
    ungetc( ch, fr );

}   
 
void skip_rest_of_line(
	FILE *fr
) {	
	int ch; 
	do { 
        ch=fgetc( fr ); 
    } while( ch!=EOF && ch!=CR && ch!=LF );
    ungetc( ch, fr );

	skip_whitespace(fr);
}  
 
void skip_comments(	
	FILE *fr
) {
	int ch;
    ch = getc( fr );        
	if ( ch==COMMENT_CHAR ) {		
		skip_rest_of_line(fr);                        
        skip_comments(fr);
	} else {
		ungetc( ch, fr );
	}
}   
 
void fsync(
	FILE *fr
){ 
	skip_whitespace(fr); 
	skip_comments(fr); 
}   

long fcopy(
	char *dest, 
	char *source
)
{
    FILE *d, *s;
    char *buffer;
    size_t incount;
    long totcount = 0L;

    s = fopen(source, "rb");
    if(s == NULL)
            return -1L;

    d = fopens(dest, "wb");
    if(d == NULL)
    {
            fclose(s);
            return -1L;
    }

    buffer = (char*) malloc(1024);
    if(buffer == NULL)
    {
            fclose(s);
            fclose(d);
            return -1L;
    }

    incount = fread(buffer, sizeof(char), 1024, s);

    while(!feof(s))
    {
            totcount += (long)incount;
            fwrite(buffer, sizeof(char), incount, d);
            incount = fread(buffer, sizeof(char), 1024, s);
    }

    totcount += (long)incount;
    fwrite(buffer, sizeof(char), incount, d);

    free(buffer);
    fclose(s);
    fclose(d);

    return totcount;
}  
 
FILE *fopens(
	char *file,
	char *mode 
){  
	if( (mode[0]=='w') || (mode[0]=='a')) {
		char drive   [256];
		char dir     [256];
		char filename[256];
		char ext     [256]; 
		_splitpath( file, drive, dir, filename, ext ); 
		mkdirs(dir);  
	}

	FILE* fp = fopen(file,mode);
	return fp;
}  
  


int count_lines(
	char *list
){
	 

	int count=0;
	FILE *fr=fopen(list,"r");
	while(1){
		fsync(fr); if(feof(fr))break;
		fgetline( fr, 0 ); 
		count++;
	}
	fclose(fr); 
	return count;
} 

char *fgetline( 
	FILE *fr, 
	int is_newline
){
	static char LINE[1024]; 
	fgets( LINE,1024, fr);

	if (is_newline==0) { //remove cr 
		for ( unsigned int i = strlen(LINE)-1; i >strlen(LINE)-3 ; i--) { 
			if ( LINE[i] == '\n' || LINE[i] == '\r' ) 
				 LINE[i] = '\0'; 
		}
	}
	return LINE;
}   


 
// directory ******************************************************************** 
 
//create directory of any depth
void mkdirs(
	char *dir 
){ 
	char opath[512]; 
	strcpy(opath, dir);

	int len = strlen(opath); 
	if(opath[len - 1] == '/') opath[len - 1] = '\0';
		 
	for(char *p = opath; *p; p++){
		if(*p == '/' || *p == '\\'){
			*p = '\0';
			if(_access(opath, 0)) _mkdir(opath);
			*p = '\\';
		}
	}
	if(_access(opath, 0)) _mkdir(opath); 
} 
 
