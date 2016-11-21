#include "common.h"
 
  
//data structure for annotation ----------------------------
const int STATE_SELECTING_X0_Y0  = 1; 
const int STATE_SELECTING_X1_Y1  = 2;
 

const int STATE_SELECTING_RY_0   = 3; 
const int STATE_SELECTING_RY_1   = 4; 
const int STATE_SELECTING_RY_2   = 5; 

const int STATE_END           =  4;
const int NUM_MAX_OBJECTS = 100;


typedef struct object{

	int x0;
	int y0;
	int x1;
	int y1; 
	float rx;
	float ry;
	float cx;
	float cy;
	float angle;

	int label;
	char classname[128];

	object( 
	){
		memset(this,0,sizeof(Object)); 
	} 
}Object;



typedef struct annotate{
   
	int state; 
	IplImage *img_color; 

	int num_objects;
	Object objects[NUM_MAX_OBJECTS];
	 
	annotate( 
	){
		memset(this,0,sizeof(Annotate)); 
	}

}Annotate;

// help functions //////////////////////////////////////////////
void show_img( 
	char* name, 
	float resize, 
	IplImage *image  
) {
	cvNamedWindow(name,0);  
	cvShowImage(name, image); 

	//automatic
	if(resize<0) {
		float s =  MAX( image->width/1280.f, image->height/720.f);
		resize = s>1? 1.f/s:1;
	}
	if(resize>=0) {
		if(image->roi)
			cvResizeWindow(name,image->roi->width*resize,image->roi->height*resize);
		else
			cvResizeWindow(name,image->width*resize,image->height*resize);
	} 
}
 
void draw_shadow_text( 
	IplImage* imgColor,
	CvPoint point, 
	CvScalar color, 
	int  thickness,
	float scale,
	const char *format, 
	... 
){ 
	va_list args ;
    va_start( args, format ) ;

	char text[512]; 
    vsprintf (text,format, args);
	 
	int x=point.x;
	int y=point.y;

    CvFont dfont;
	CvFont dfont1;
    float hscale      = scale;//0.40f;
    float vscale      = scale;//0.40f;
    float italicscale = 0.0f;
     
    cvInitFont (&dfont, CV_FONT_HERSHEY_SIMPLEX  , hscale, vscale, italicscale, thickness,   CV_AA);
    cvInitFont (&dfont1, CV_FONT_HERSHEY_SIMPLEX , hscale, vscale, italicscale, thickness*3, CV_AA);
 
	cvPutText(imgColor, text, cvPoint(x, y), &dfont1, cvScalar(64,64,64));	//cvScalar(64,64,64) //cvScalar(255,255,255)
	cvPutText(imgColor, text, cvPoint(x, y), &dfont,  color );
 
}   

void draw_object(
	IplImage *img_color,
	Object *o,
	CvScalar color,
	int thickness,
	int is_label 
){
	cvEllipse(img_color, cvPoint(o->cx,o->cy), cvSize(o->rx,o->ry), o->angle, 0, 360, color, thickness,CV_AA);
	cvLine   (img_color, cvPoint(o->x0,o->y0), cvPoint(o->x1,o->y1), color, 1, CV_AA);
	cvCircle (img_color, cvPoint(o->x1,o->y1), 2.5*thickness +0.5, color, -1, CV_AA);

	if(is_label) draw_shadow_text(img_color,cvPoint(o->cx,o->cy),cvScalar(255,255,255),1,0.6,"%d: %s", o->label, o->classname );
}

void on_mouse(
	int event, 
	int x, 
	int y, 
	int flags,
	void *param
){
	Annotate *a = (Annotate*) param;
	if (a->img_color==0) return;

	IplImage *img_color = cvCloneImage(a->img_color);
	Object *o = &a->objects[a->num_objects]; 
	ASSERT(a->num_objects<NUM_MAX_OBJECTS,"");
	 
	if(a->state==STATE_SELECTING_X0_Y0){
		if(event == CV_EVENT_LBUTTONDOWN){
			//printf("x=%d,  y=%d\n", x,y);  
			o->x0=x; 
			o->y0=y; 
			a->state=STATE_SELECTING_X1_Y1;
		}
	}

	if(a->state==STATE_SELECTING_X1_Y1){ 
		if(event == CV_EVENT_MOUSEMOVE){
			o->x1=x; 
			o->y1=y; 
			cvLine(img_color, cvPoint(o->x0,o->y0), cvPoint(o->x1,o->y1), cvScalar(0,0,255), 1, CV_AA);
			cvCircle(img_color, cvPoint(o->x1,o->y1), 5, cvScalar(0,0,255), -1, CV_AA); 
		}
		if(event == CV_EVENT_LBUTTONUP){ 
			o->x1 = x; 
			o->y1 = y; 
			float dx = o->x1-o->x0;
			float dy = o->y1-o->y0; 

			o->cx = (o->x0+o->x1)/2; 
			o->cy = (o->y0+o->y1)/2;  
			o->angle = atan2(dy,dx)/PI *180; 
			o->rx    = sqrtf(dx*dx+dy*dy)/2; 
			 
			cvLine(img_color, cvPoint(o->x0,o->y0), cvPoint(o->x1,o->y1), cvScalar(0,0,255), 1, CV_AA);
			cvCircle(img_color, cvPoint(o->x1,o->y1), 3, cvScalar(0,0,255  ), -1, CV_AA); 
			cvCircle(img_color, cvPoint(o->cx,o->cy), 3, cvScalar(0,255,255),  1, CV_AA); 
			 
			cvCopy(img_color,a->img_color);
			a->state=STATE_SELECTING_RY_0; 
		}
	}
	
	if(a->state==STATE_SELECTING_RY_0){  
		if(event == CV_EVENT_LBUTTONDOWN){
			//printf("x=%d,  y=%d\n", x,y);   
			a->state=STATE_SELECTING_RY_1; 
		}
	}

	if(a->state==STATE_SELECTING_RY_1){  
		if (event == CV_EVENT_MOUSEMOVE) {	
			cvLine(img_color, cvPoint(o->cx,o->cy), cvPoint(x,y), cvScalar(0,255,255), 1, CV_AA);

			float c = cosf(o->angle/180.f *PI);
			float s = sinf(o->angle/180.f *PI); 
			float dx = x-o->cx;
			float dy = y-o->cy;
			o->ry=fabs(dx*s-c*dy); 
			draw_object( img_color, o, cvScalar(0,0,255),1,0);
		} 

		if(event == CV_EVENT_LBUTTONUP){
			draw_object( img_color, o, cvScalar(0,0,255),1,0);  
			cvCopy(img_color,a->img_color);
			a->state=STATE_SELECTING_RY_2;
		} 
	}


	show_img("SELECT", 1, img_color);
	cvReleaseImage(&img_color);
}


 
//start here! ///////////////////////////////////////////////////////////////
void run_annotate( 
){

	//data dir
	char img_dir[512]        ="F:\\[Kaggle]\\[fishery]\\[data]\\[dummy]\\set00\\imgs\\YFT";
	char annotation_dir[512] ="F:\\[Kaggle]\\[fishery]\\[data]\\[dummy]\\set00\\annotations\\YFT";
	char list_file[512]      ="F:\\[Kaggle]\\[fishery]\\[data]\\[dummy]\\set00\\YFT.txt";
	char save_dir[512]       ="F:\\[Kaggle]\\[fishery]\\[data]\\[dummy]\\set00\\~marks\\YFT";
	 
	////char img_dir[512]        ="F:\\[Kaggle]\\[fishery]\\[data]\\[TNC]\\imgs\\ALB";
	////char annotation_dir[512] ="F:\\[Kaggle]\\[fishery]\\[data]\\[TNC]\\annotations\\ALB";
	////char list_file[512]      ="F:\\[Kaggle]\\[fishery]\\[data]\\[TNC]\\ALB.txt";
	////char save_dir[512]       ="F:\\[Kaggle]\\[fishery]\\[data]\\[TNC]\\~marks\\ALB";
	//---------------------------------------------------------------------------------------------
	char classnames[][128]={
		 
		              //1-DOL   2-LAG            4-ALB             5-BET            6-YFT
		"background", "Mahi", "Opah", "Shark", "Albacore_tuna", "Bigeye_tuna", "Yellowfin_tuna", "unknown?"
	};
	int label = 4; //set your label here!
		
	  
	//prepare 
	mkdirs(save_dir);

	Annotate *a = new Annotate();//data structure for annotation
	for (int n=0;n<NUM_MAX_OBJECTS;n++){
		a->objects[n].label=label;
		strcpy(a->objects[n].classname,classnames[label]);
	}
	cvNamedWindow("SELECT",0);
	cvSetMouseCallback("SELECT", on_mouse, a);
	 

	FILE *fr = fopens(list_file,"r");
	while(1){
		fsync(fr); if (feof(fr)) break;

		char name[64];
		fscanf(fr,"%s\n",name);
		IplImage *img_color = cvLoadImage(str(0,"%s\\%s.jpg",img_dir,name),1);
		show_img("SELECT", 1, img_color);  
		cvWaitKey(1);

		char logfile[512]; 
		remove(str(logfile,"%s\\%s.txt",annotation_dir,name));
		printf("name=%s\n",name);
		printf_fprintf(logfile,"#format: x0, y0, x1, y1,  cx, cy, rx, ry,   angle,  label #classname\n");

		//reset
		a->img_color=cvCloneImage(img_color);
		a->num_objects=0;
		a->state=STATE_SELECTING_X0_Y0;

		 
		while(1){
			int key=cvWaitKey(0);
			//printf("key=%d\n",key);
		 
			if(key==32){ //'space' 
				if(a->state != STATE_SELECTING_RY_2) continue;
				 
				//save
				Object *o = &a->objects[a->num_objects];
				printf_fprintf(logfile,"%d\t%d\t%d\t%d\t %0.1f\t%0.1f\t%0.1f\t%0.1f\t%0.1f\t%d  #%s\n", o->x0, o->y0, o->x1, o->y1,  o->cx, o->cy, o->rx, o->ry, o->angle, label,classnames[label]);
				 
				draw_object( img_color, o, cvScalar(0,255,255),4,0);
				draw_object( img_color, o, cvScalar(0,0,255),2,1);
				cvCopy(img_color,a->img_color); 
				a->num_objects++; 
				a->state = STATE_SELECTING_X0_Y0; 
			}

			if(key==101){ //'e'  : erase last   
				a->img_color=cvCloneImage(img_color);
				a->state=STATE_SELECTING_X0_Y0;
			}
			if(key==13){ //'enter'  : next image 
				break;
			}
		}
		printf_fprintf(logfile,"#num_objects = %d\n",a->num_objects);
		printf("\n");
		cvSaveImage(str(0,"%s\\%s.jpg",save_dir,name),img_color);
		cvReleaseImage(&img_color);
	}
	fclose(fr);
}


void show_annotate( 
){
	int is_save=1;


	//data dir
	char img_dir[512]        ="F:\\[Kaggle]\\[fishery]\\[data]\\[dummy]\\set00\\imgs\\ALB";
	char annotation_dir[512] ="F:\\[Kaggle]\\[fishery]\\[data]\\[dummy]\\set00\\annotations\\ALB";
	char list_file[512]      ="F:\\[Kaggle]\\[fishery]\\[data]\\[dummy]\\set00\\ALB.txt"; 
	char save_dir[512]       ="F:\\[Kaggle]\\[fishery]\\[data]\\[dummy]\\set00\\~marks\\ALB";


	if(is_save) mkdirs(save_dir); 
	Object objects[NUM_MAX_OBJECTS];

	FILE *fr = fopens(list_file,"r");
	while(1){
		fsync(fr); if (feof(fr)) break;

		char name[64];
		fscanf(fr,"%s\n",name);
		IplImage *img_color = cvLoadImage(str(0,"%s\\%s.jpg",img_dir,name),1);
		 
		char logfile[512]; 
		int num_objects = count_lines (str(logfile,"%s\\%s.txt",annotation_dir,name)); 
		FILE *fr1 = fopens(logfile,"r");
		for (int n=0; n<num_objects; n++){ 
			Object *o = &objects[n];

			//printf_fprintf(logfile,"#format: cx, cy, rx, ry, angle, label  #classname\n");
			fsync(fr1); fscanf(fr1,"%d %d %d %d %f %f %f %f %f %d %s\n", &o->x0, &o->y0, &o->x1, &o->y1, &o->cx, &o->cy, &o->rx, &o->ry, &o->angle, &o->label, o->classname );
			draw_object( img_color, o, cvScalar(0,255,255),4,0);
			draw_object( img_color, o, cvScalar(0,0,255),2,1);
		}
		fclose(fr1);
		draw_shadow_text(img_color,cvPoint(10,20),cvScalar(255,255,255),1,0.7,"%s", name );	
		draw_shadow_text(img_color,cvPoint(10,40),cvScalar(255,255,255),1,0.7,"num_objects = %d", num_objects );	

		show_img("SELECT", 1, img_color);  
		cvWaitKey(1);

		if(save_dir) cvSaveImage(str(0,"%s\\%s.jpg",save_dir,name),img_color);
		cvReleaseImage(&img_color);
	}
	fclose(fr);
}
