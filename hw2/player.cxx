#ifdef WIN32
#include <FL/gl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>				// Add string functionality
#include <fstream>
#include <assert.h>
#include <math.h>
#include <process.h>
#include <vector>
#include <iostream>

#include <GL/gl.h>				// Header so that you can use GL routines (MESA)
#include <GL/glu.h>				// some OpenGL extensions
#include <FL/glut.H>			// GLUT for use with FLTK
#include <FL/fl_file_chooser.H> // Allow a file chooser for save.

#include "player.h"   		      
#include "interface.h"			// UI framework built by FLTK (using fluid)
#include "transform.h"			// utility functions for vector and matrix transformation  
#include "display.h"   
#include "interpolator.h"
#include "MotionGraph.h"

using namespace std;

/***************  Types *********************/
enum {OFF, ON};


/***************  Static Variables *********/
static Display displayer;		

static Skeleton *pActor = NULL;			// Actor info as read from ASF file
static bool bActorExist = false;		// Set to true if actor exists

static Motion *pSampledMotion = NULL;	// Motion information as read from AMC file
static Motion *pInterpMotion = NULL;	// Interpolated Motion 


static int nFrameNum, nFrameInc=1;		// Current frame and frame increment

static Fl_Window *form=NULL;  			// Global form 
static MouseT mouse;					// Keeping track of mouse input 
static CameraT camera;					// Structure about camera setting 

static int Play=OFF, Rewind=OFF;		// Some Flags for player
static int Repeat=OFF, Record=OFF; 

static int PlayInterpMotion=ON;			// Flag which desides which motion to play (pSampledMotion or pInterpMotion)	

static int Background=ON, Light=OFF;	// Flags indicating if the object exists    

static char *Record_filename;			// Recording file name 

static int recmode = 0;
static int piccount=0;
static char * argv2;
static int maxFrames=0;

std::vector <double*> line;
bool path_draw = true;

static MotionGraph *pMotionGraph = NULL;
int CurrentFrame = -1;

//	Variables for FPS calculation
int Frames = 0;
const int UpdateTime = 1000;
int LastTime = 0, CurrentTime = 0;
double FPS = 0.0f;

/***************  Functions *******************/
static void draw_triad() 
{
    glBegin(GL_LINES);

   /* draw x axis in red, y axis in green, z axis in blue */
   glColor3f(1., .2, .2);
   glVertex3f(0., 0., 0.);
   glVertex3f(1., 0., 0.);

   glColor3f(.2, 1., .2);
   glVertex3f(0., 0., 0.);
   glVertex3f(0., 1., 0.);

   glColor3f(.2, .2, 1.);
   glVertex3f(0., 0., 0.);
   glVertex3f(0., 0., 1.);

   glEnd();
}

//Draw checker board ground plane
static void draw_ground() 
{
   float i, j;
   int count = 0;

   GLfloat white4[] = {.4, .4, .4, 1.};
   GLfloat white1[] = {.1, .1, .1, 1.};
   GLfloat green5[] = {0., .5, 0., 1.};
   GLfloat green2[] = {0., .2, 0., 1.};
   GLfloat black[] = {0., 0., 0., 1.};
   GLfloat mat_shininess[] = {7.};		/* Phong exponent */
	
   glBegin(GL_QUADS);

   for(i=-15.;i<=15.;i+=1) 
   {
      for(j=-15.;j<=15.;j+=1) 
      {			 
		if((count%2) == 0) 
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, black);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white4);
//			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white1);
//			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
			glColor3f(.6, .6, .6);
		}
		else 
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, black);	
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, green5);
//			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, green2);
//			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
			glColor3f(.8, .8, .8);
		}

		glNormal3f(0.,0.,1.);

		glVertex3f(j,  0, i);
		glVertex3f(j,  0, i+1);
		glVertex3f(j+1,0, i+1);
		glVertex3f(j+1,0, i);
		count++;
	  }
   }

   glEnd();
}

double* GetOGLPos(int x, int y)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	double* pos = new double[3];
	pos[0] = posX;
	pos[1] = posY;
	pos[2] = posZ;

	return pos;
}

void cameraView(void)
{
    glTranslated(camera.tx, camera.ty, camera.tz);
    glTranslated(camera.atx, camera.aty, camera.atz);

    glRotated(-camera.tw, 0.0, 1.0, 0.0);
    glRotated(-camera.el, 1.0, 0.0, 0.0);
    glRotated(camera.az, 0.0, 1.0, 0.0); 
    
	glTranslated(-camera.atx, -camera.aty, -camera.atz);
    glScaled(camera.zoom, camera.zoom, camera.zoom);
}





/*
 * redisplay() is called by Player_Gl_Window::draw().
 *
 * The display is double buffered, and FLTK swap buffers when
 * Player_Gl_Window::draw() returns.  The GL context associated with this
 * instance of Player_Gl_Window is set to be the current context by FLTK
 * when it calls draw().
 */
static void redisplay() 
{
    if(Light) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);

    /* clear image buffer to black */
    glClearColor(0, 0, 0, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); /* clear image, zbuf */

    glPushMatrix();			/* save current transform matrix */
 
    cameraView();

    glLineWidth(2.);		/* we'll draw background with thick lines */

    if (Background)
       draw_ground();

	if(mouse.button==1 && path_draw)
	{
		double* temp = GetOGLPos(mouse.x, mouse.y);
		if(line.size())
		{
			double* temp2 = line[line.size()-1];
			if(temp[0]!=temp2[0] || temp[2]!=temp2[2])
				line.push_back(temp);
		}
		else
			line.push_back(temp);
	}
    
	for(int i=1;i<line.size();i++)
	{
		glBegin(GL_LINES);

	   glColor3f(1., 0.0f, 0.0f);
	   double* temp1 = line[i-1];
	   double* temp2 = line[i];
	   glVertex3f(line[i-1][0], 0.002f, line[i-1][2]);

	   glVertex3f(line[i  ][0], 0.002f, line[i  ][2]);

	   glEnd();
	}

	if(Background)
		draw_triad();		/* draw a triad in the origin of the world coord */

    if (bActorExist) displayer.show();

    glPopMatrix();			/* restore current transform matrix */

}

/* Callbacks from form. */
void redisplay_proc(Fl_Light_Button *obj, long val) 
{
	Light = light_button->value();
	Background  = background_button->value();
	glwindow->redraw();
}


void switchmode_proc(Fl_Light_Button *obj, long val) 
{
	path_draw = !path_draw;
}


//Interpolate loaded motion using linear interpolation
void interpolate_callback(Fl_Button *button, void *) 
{

	if (pMotionGraph)
	if (line.size() > 1)
	{
		pMotionGraph->doPathSynthesis(line);
	}
	else
	{
		CurrentFrame = pMotionGraph->m_MaxSCCRoot->m_FrameIndex;
		displayer.m_pActor[0]->setPosture(pMotionGraph->m_pPostures[CurrentFrame]);
	}
}


void load_callback(Fl_Button *button, void *) 
{
	char *filename;

	if(button==loadActor_button)
	{
		filename = fl_file_chooser("Select filename","*.ASF","");
		if(filename != NULL)
		{
			//Remove old actor
			//if(pActor != NULL) 
			//	delete pActor; 
			//Read skeleton from asf file
			pActor = new Skeleton(filename, MOCAP_SCALE);

			//Set the rotations for all bones in their local coordinate system to 0
			//Set root position to (0, 0, 0)
			pActor->setBasePosture();
			displayer.loadActor(pActor);
			bActorExist = true;
			glwindow->redraw();
		}
	}

	if(button==loadMotion_button)
	{
		if (bActorExist == true)
		{
			filename = fl_file_chooser("Select filename","*.AMC","");
			if(filename != NULL)
			{
				//delete old motion if any
				//if (pSampledMotion != NULL)
				//{
				//	delete pSampledMotion;
				//	pSampledMotion = NULL;
				//}
				//if (pInterpMotion != NULL)
				//{
				//	delete pInterpMotion;
				//	pInterpMotion = NULL;
				//}
				

				//Read motion (.amc) file and create a motion
				pSampledMotion = new Motion(filename, MOCAP_SCALE,pActor);

				//set sampled motion for display
				displayer.loadMotion(pSampledMotion);               
			
				//Tell actor to perform the first pose ( first posture )
//				pActor->setPosture(displayer.m_pMotion->m_pPostures[0]);          
				maxFrames = 0;
				if ( (displayer.m_pMotion[displayer.numActors-1]->m_NumFrames - 1) > maxFrames)
				{
					maxFrames = (displayer.m_pMotion[displayer.numActors-1]->m_NumFrames - 1);
					frame_slider->maximum((double)maxFrames+1);

				}
				nFrameNum=(int) frame_slider->value() -1;

				// display
				for (int i = 0; i < displayer.numActors; i++)
					displayer.m_pActor[i]->setPosture(displayer.m_pMotion[i]->m_pPostures[displayer.m_pMotion[i]->GetPostureNum(nFrameNum)]);
					
				Fl::flush();
				glwindow->redraw();
			}
		}

	}
	glwindow->redraw();
}

void save_callback(Fl_Button *button, void *) 
{
	//char *filename;
	if(button==save_button)
		glwindow->save(fl_file_chooser("Save to Jpeg File", "*.jpg", ""));

}

void play_callback(Fl_Button *button, void *)
{
	if(displayer.m_pMotion[0] != NULL)
	{
		if(button==play_button) { Play=ON; Rewind=OFF; }
		if(button==pause_button){ Play=OFF; Repeat=OFF; } 
		if(button==repeat_button) { Rewind=OFF; Play=ON; Repeat=ON; }
		if(button==rewind_button) { Rewind=ON; Play=OFF; Repeat=OFF; }
	}
}

void record_callback(Fl_Light_Button *button, void *)
{
	int current_state = (int) button->value();

	if(Play == OFF)
	{
		if(Record == OFF && current_state == ON)
		{
			Record_filename = fl_file_chooser("Save Animation to Jpeg Files", "", "");
			if(Record_filename != NULL)
				Record = ON;
		}
		if(Record == ON && current_state == OFF) 
			Record = OFF;

	}
	button->value(Record);
}

void idle(void*)
{
	bool flag = true;

	
	
	if (line.size() && pMotionGraph->buffer.size())
	{
		displayer.m_pActor[0]->setPosture(pMotionGraph->buffer[pMotionGraph->m_BufferIndex]);
		if (pMotionGraph->m_BufferIndex+1 < pMotionGraph->buffer.size())
			pMotionGraph->m_BufferIndex++;
	}


	if(CurrentFrame != -1)
	{
		printf("CurrentFrame = %d\n", CurrentFrame);
		printf("buffer size = %d\n", pMotionGraph->buffer.size());
		CurrentFrame = pMotionGraph->Traverse1(CurrentFrame, flag);
		if (pMotionGraph->buffer.size() > 0)
		{
			displayer.m_pActor[0]->setPosture(pMotionGraph->buffer[pMotionGraph->m_BufferIndex]);

			if (pMotionGraph->m_BufferIndex+1 < pMotionGraph->buffer.size())
				pMotionGraph->m_BufferIndex++;

			if (pMotionGraph->buffer.size() > 1000)
			{
				pMotionGraph->GenerateMotion(1000, 0, "OutputMotion.amc");
				exit(0);
			}

		}
	}
	/*else
	if (displayer.m_pMotion[0] != NULL)
	{
		if(Rewind==ON)
		{
			nFrameNum=0;
			for (int i = 0; i < displayer.numActors; i++)
				displayer.m_pActor[i]->setPosture(displayer.m_pMotion[i]->m_pPostures[displayer.m_pMotion[i]->GetPostureNum(nFrameNum)]);
			Rewind=OFF;
		}

		if(Play==ON) 
		{
			if(nFrameNum >= maxFrames)  
			{
				if(Repeat == ON)
				{
					nFrameNum=0;
					for (int i = 0; i < displayer.numActors; i++)
						displayer.m_pActor[i]->setPosture(displayer.m_pMotion[i]->m_pPostures[displayer.m_pMotion[i]->GetPostureNum(nFrameNum)]);
				}
				else
				{
					for (int i = 0; i < displayer.numActors; i++)
						displayer.m_pActor[i]->setPosture(displayer.m_pMotion[i]->m_pPostures[displayer.m_pMotion[i]->GetPostureNum(nFrameNum)]);
				}
			}
			else
				 for (int i = 0; i < displayer.numActors; i++)
					 displayer.m_pActor[i]->setPosture(displayer.m_pMotion[i]->m_pPostures[displayer.m_pMotion[i]->GetPostureNum(nFrameNum)]);

			if(Record==ON) 
				glwindow->save(Record_filename);
			
			if (nFrameNum < maxFrames)
				nFrameNum += nFrameInc;
		}
	}*/

	frame_slider->value((double)nFrameNum+1);
	glwindow->redraw();

	//	Calculate FPS
	Frames++;
	if ( (GetTickCount() - LastTime) > UpdateTime )
	{
		FPS = ((double)Frames/(double)(GetTickCount()-LastTime)) * 1000.0f;
		LastTime = GetTickCount();
		Frames = 0;
	}
	//	Adjust to 30 fps
	while ( (GetTickCount() - CurrentTime) < 30)
	{}
	CurrentTime = GetTickCount();

	//printf("FPS:%f\n", FPS);
}

void fslider_callback(Fl_Value_Slider *slider, long val)
{
	if (displayer.m_pMotion[0] != NULL)
	{
		if(displayer.m_pMotion[0]->m_NumFrames > 0) 
		{
			nFrameNum=(int) frame_slider->value()-1;
			for (int i = 0; i < displayer.numActors; i++)
//				if(displayer.m_pMotion[i]->m_NumFrames > 0)
					displayer.m_pActor[i]->setPosture(displayer.m_pMotion[i]->m_pPostures[displayer.m_pMotion[i]->GetPostureNum(nFrameNum)]);
			Fl::flush();
			glwindow->redraw();
		}
	}
}

// locate rotation center at the (root.x, 0, root.z)
void locate_callback(Fl_Button *obj, void *)
{
   if(bActorExist && displayer.m_pMotion[0] != NULL)
   {
      camera.zoom = 1;
      camera.atx = pActor->m_RootPos[0];  
      camera.aty = 0;
      camera.atz = pActor->m_RootPos[2];
   }
   glwindow->redraw();
}

void valueIn_callback(Fl_Value_Input *obj, void *)
{
    displayer.m_SpotJoint = (int) joint_idx->value();
    nFrameInc = (int) fsteps->value();
    glwindow->redraw();
}

void sub_callback(Fl_Value_Input *obj, void*)
{
	int subnum;
	subnum = (int)sub_input->value();
	if (subnum < 0) sub_input->value(0);
	else if (subnum > displayer.numActors-1) sub_input->value(displayer.numActors-1);
	else
	{
		// Change values of other inputs to match subj num
		dt_input->value(displayer.m_pMotion[subnum]->offset);
		tx_input->value(displayer.m_pActor[subnum]->tx);
		ty_input->value(displayer.m_pActor[subnum]->ty);
		tz_input->value(displayer.m_pActor[subnum]->tz);
		rx_input->value(displayer.m_pActor[subnum]->rx);
		ry_input->value(displayer.m_pActor[subnum]->ry);
		rz_input->value(displayer.m_pActor[subnum]->rz);
	}
	glwindow->redraw();
}

void dt_callback(Fl_Value_Input *obj, void*)
{
	int subnum,max = 0;
	subnum = (int)sub_input->value();
	if (subnum < displayer.numActors && subnum >= 0)
	{
		displayer.m_pMotion[subnum]->SetTimeOffset((int)dt_input->value());	
		printf("Shifting subject %d by %d\n",subnum,(int)dt_input->value());
		for (int i = 0; i < displayer.numActors; i++)
		{
			if ((displayer.m_pMotion[i]->m_NumFrames - 1 - displayer.m_pMotion[i]->offset) > max)
				max = (displayer.m_pMotion[i]->m_NumFrames - 1 - displayer.m_pMotion[i]->offset);
		}
		maxFrames = max;
		frame_slider->maximum((double)maxFrames+1);
		displayer.m_pActor[subnum]->setPosture(displayer.m_pMotion[subnum]->m_pPostures[displayer.m_pMotion[subnum]->GetPostureNum(nFrameNum)]);
	}
	glwindow->redraw();
}


void tx_callback(Fl_Value_Input *obj, void*)
{
	int subnum = 0;
	subnum = (int)sub_input->value();
	if (subnum < displayer.numActors && subnum >= 0)
	{
		displayer.m_pActor[subnum]->tx = (int)tx_input->value();
	}
	glwindow->redraw();
}

void ty_callback(Fl_Value_Input *obj, void*)
{
	int subnum = 0;
	subnum = (int)sub_input->value();
	if (subnum < displayer.numActors && subnum >= 0)
	{
		displayer.m_pActor[subnum]->ty = (int)ty_input->value();
	}
	glwindow->redraw();
}

void tz_callback(Fl_Value_Input *obj, void*)
{
	int subnum = 0;
	subnum = (int)sub_input->value();
	if (subnum < displayer.numActors && subnum >= 0)
	{
		displayer.m_pActor[subnum]->tz = (int)tz_input->value();
	}
	glwindow->redraw();
}

void rx_callback(Fl_Value_Input *obj, void*)
{
	int subnum = 0;
	subnum = (int)sub_input->value();
	if (subnum < displayer.numActors && subnum >= 0)
	{
		displayer.m_pActor[subnum]->rx = (int)rx_input->value();
	}
	glwindow->redraw();
}

void ry_callback(Fl_Value_Input *obj, void*)
{
	int subnum = 0;
	subnum = (int)sub_input->value();
	if (subnum < displayer.numActors && subnum >= 0)
	{
		displayer.m_pActor[subnum]->ry = (int)ry_input->value();
	}
	glwindow->redraw();
}

void rz_callback(Fl_Value_Input *obj, void*)
{
	int subnum = 0;
	subnum = (int)sub_input->value();
	if (subnum < displayer.numActors && subnum >= 0)
	{
		displayer.m_pActor[subnum]->rz = (int)rz_input->value();
	}
	glwindow->redraw();
}

void exit_callback(Fl_Button *obj, long val) 
{
	//DEBUG: uncomment
	exit(1);
}

void light_init() 
{
    /* set up OpenGL to do lighting
     * we've set up three lights */

    /* set material properties */
    GLfloat white8[] = {.8, .8, .8, 1.};
    GLfloat white2[] = {.2, .2, .2, 1.};
    GLfloat black[] = {0., 0., 0., 1.};
    GLfloat mat_shininess[] = {50.};		/* Phong exponent */

    GLfloat light0_position[] = {-25., 25., 25., 0.}; /* directional light (w=0) */
    GLfloat white[] = {11., 11., 11., 5.};

    GLfloat light1_position[] = {-25., 25., -25., 0.};
    GLfloat red[] = {1., .3, .3, 5.};

    GLfloat light2_position[] = {25., 25., -5., 0.};
    GLfloat blue[] = {.3, .4, 1., 25.};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white2);	/* no ambient */
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white8);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

    /* set up several lights */
    /* one white light for the front, red and blue lights for the left & top */

    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, red);
    glLightfv(GL_LIGHT1, GL_SPECULAR, red);
    glEnable(GL_LIGHT1);

    glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, blue);
    glLightfv(GL_LIGHT2, GL_SPECULAR, blue);
    glEnable(GL_LIGHT2);

//mstevens
	GLfloat light3_position[] = {0., -25., 0., 0.6};
    glLightfv(GL_LIGHT3, GL_POSITION, light3_position);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT3, GL_SPECULAR, white);
    glEnable(GL_LIGHT3);

    glEnable(GL_NORMALIZE);	/* normalize normal vectors */
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);	/* two-sided lighting*/

    /* do the following when you want to turn on lighting */
    if(Light) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);
}


static void error_check(int loc) 
{
    /* this routine checks to see if OpenGL errors have occurred recently */
    GLenum e;

    while ((e = glGetError()) != GL_NO_ERROR)
	fprintf(stderr, "Error: %s before location %d\n",
	    gluErrorString(e), loc);
}


void gl_init() 
{
    int red_bits, green_bits, blue_bits;
    struct {GLint x, y, width, height;} viewport;
    glEnable(GL_DEPTH_TEST);	/* turn on z-buffer */

    glGetIntegerv(GL_RED_BITS, &red_bits);
    glGetIntegerv(GL_GREEN_BITS, &green_bits);
    glGetIntegerv(GL_BLUE_BITS, &blue_bits);
    glGetIntegerv(GL_VIEWPORT, &viewport.x);
    printf("OpenGL window has %d bits red, %d green, %d blue; viewport is %dx%d\n",
	red_bits, green_bits, blue_bits, viewport.width, viewport.height);

    /* setup perspective camera with OpenGL */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(/*vertical field of view*/ 45.,
	/*aspect ratio*/ (double) viewport.width/viewport.height,
	/*znear*/ .1, /*zfar*/ 50.);

    /* from here on we're setting modeling transformations */
    glMatrixMode(GL_MODELVIEW);
    
	//Move away from center
	glTranslatef(0., 0., -5.);
    
	camera.zoom = 1;

	camera.tw = 0;
	camera.el = -15;
    camera.az = -25;
    
    camera.atx = 0;
    camera.aty = 0;
    camera.atz = 0;
}


/*******************
 * Define the methods for glwindow, a subset of Fl_Gl_Window. 
 *******************/

/*
 * Handle keyboard and mouse events.  Don't make any OpenGL calls here;
 * the GL Context is not set!  Make the calls in redisplay() and call
 * the redraw() method to cause FLTK to set up the context and call draw().
 * See the FLTK documentation under "Using OpenGL in FLTK" for additional
 * tricks and tips.
 */
int Player_Gl_Window::handle(int event) 
{
   int handled = 1;
   static int prev_x, prev_y;
   int delta_x=0, delta_y=0; 
   float ev_x, ev_y;
 
   switch(event) {
   case FL_RELEASE:
      mouse.x = (Fl::event_x());
      mouse.y = (Fl::event_y());
      mouse.button = 0;
      break;
   case FL_PUSH:
      mouse.x = (Fl::event_x());
      mouse.y = (Fl::event_y());
      mouse.button = (Fl::event_button());
       break;
   case FL_DRAG:
      mouse.x = (Fl::event_x());
      mouse.y = (Fl::event_y());
      delta_x=mouse.x-prev_x;
      delta_y=mouse.y-prev_y; 
	  
      if(mouse.button == 1 && !path_draw)
      { 
         if(abs(delta_x) > abs(delta_y))
	    camera.az += (GLdouble) (delta_x);          
         else
            camera.el -= (GLdouble) (delta_y);
      }
      else if(mouse.button==2)
      {
        if(abs(delta_y) > abs(delta_x))
        {
		glScalef(1+delta_y/100.,1+delta_y/100.,1+delta_y/100.);	
       //     camera.zoom -= (GLdouble) delta_y/100.0;
       //     if(camera.zoom < 0.) camera.zoom = 0;
		}
      }
      else if(mouse.button==3){
         //camera.tx += (GLdouble) delta_x/10.0;
         //camera.tz -= (GLdouble) delta_y/10.0; //FLTK's origin is at the left_top corner
		 
         camera.tx += (GLdouble) cos(camera.az/180.0*3.141)*delta_x/10.0;
		 camera.tz += (GLdouble) sin(camera.az/180.0*3.141)*delta_x/10.0;
         camera.ty -= (GLdouble) delta_y/10.0; //FLTK's origin is at the left_top corner

		 camera.atx = -camera.tx;
		 camera.aty = -camera.ty;
		 camera.atz = -camera.tz;
      }
      break;
   case FL_KEYBOARD:
      switch (Fl::event_key()) {
      case 'q':
      case 'Q':
      case 65307:
         exit(0);
      }
      break;
   default:
      // pass other events to the base class...
      handled= Fl_Gl_Window::handle(event);
   }

   prev_x=mouse.x;
   prev_y=mouse.y;
   glwindow->redraw();

   return (handled);  // Returning one acknowledges that we handled this event
}


/*
	Prewritten Save Function
*/
void Player_Gl_Window::save (char *filename) 
{
	/*int i;
	int j;
	static char anim_filename[512];
	static Pic *in = NULL;

	sprintf(anim_filename, "%05d.jpg", piccount++); 
	if (filename == NULL) return;

	//Allocate a picture buffer.
	if(in == NULL) in = pic_alloc(640,480,3,NULL);
  
	printf("File to save to: %s\n", anim_filename);

	for (i=479; i>=0; i--) 
	{
		glReadPixels(0,479-i,640,1,GL_RGB, GL_UNSIGNED_BYTE, 
					   &in->pix[i*in->nx*in->bpp]);
   }

	if (jpeg_write(anim_filename, in))
		printf("%s saved Successfully\n", anim_filename);
	else
		printf("Error in Saving\n");*/
}


/*
	Prewritten Draw Function.  
*/
void Player_Gl_Window::draw () 
{
	//Upon setup of the window (or when Fl_Gl_Window->invalidate is called), 
	//the set of functions inside the if block are executed.
	if (!valid()) 
	{
/*		if (fopen("Skeleton.ASF", "r") == NULL)
		{ 
			printf("Program can't run without 'Skeleton.ASF'!.\n"
				   "Please make sure you place a 'Skeleton.ASF' file to working directory.\n");
			exit(1);
		}*/
		gl_init();
		light_init();    
	}

	//Redisplay the screen then put the proper buffer on the screen.
	redisplay();
}


int main(int argc, char **argv) 
{

    /* initialize form, sliders and buttons*/
    form = make_window();

    light_button->value(Light);
    background_button->value(Background);
    record_button->value(Record);

    frame_slider->value(1);

    /*show form, and do initial draw of model */
    form->show();
    glwindow->show(); /* glwindow is initialized when the form is built */
if (argc > 2)
{
	char *filename;

	if(1==1)
	{
		filename = argv[1];
		if(filename != NULL)
		{
			//Remove old actor
			if(pActor != NULL) 
				delete pActor; 
			//Read skeleton from asf file
			pActor = new Skeleton(filename, MOCAP_SCALE);

			//Set the rotations for all bones in their local coordinate system to 0
			//Set root position to (0, 0, 0)
			pActor->setBasePosture();
			displayer.loadActor(pActor);
			bActorExist = true;
		}
	}

	if(1==1)
	{
		if (bActorExist == true)
		{
			argv2 = filename = argv[2];
			if(filename != NULL)
			{
				//delete old motion if any
				if (pSampledMotion != NULL)
				{
					delete pSampledMotion;
					pSampledMotion = NULL;
				}
				if (pInterpMotion != NULL)
				{
					delete pInterpMotion;
					pInterpMotion = NULL;
				}

				//--- Create a motion graph from a batch of parameters ---
				if (argc > 3)
				{
					pMotionGraph = new MotionGraph();
					pMotionGraph->setActor(pActor);
					for (int i = 2; i < argc; i++)
					{
						pSampledMotion = new Motion(argv[i], MOCAP_SCALE, pActor);
						pMotionGraph->Concatenate(*pSampledMotion);
						delete pSampledMotion;
					}
					cout << "NumMotions in MotionGraph = " << pMotionGraph->m_NumMotions << endl;
					pMotionGraph->Construct();
				}
		

				//Read motion (.amc) file and create a motion
				pSampledMotion = new Motion(filename, MOCAP_SCALE,pActor);

				//set sampled motion for display
				displayer.loadMotion(pSampledMotion);            

				maxFrames = (displayer.m_pMotion[displayer.numActors-1]->m_NumFrames - 1);
				frame_slider->maximum((double)maxFrames+1);
			
				//Tell actor to perform the first pose ( first posture )
				pActor->setPosture(displayer.m_pMotion[0]->m_pPostures[0]);          

				//frame_slider->maximum((double)displayer.m_pMotion[0]->m_NumFrames );

				nFrameNum=0;
				
			}
		}
		else
			printf("Load Actor first.\n");
				nFrameInc=4;            // Current frame and frame increment
Play=ON;               // Some Flags for player
Repeat=OFF;
Record=ON;
//Background=OFF; 
Light=OFF; // Flags indicating if the object exists
Record_filename = "";                    // Recording file name
recmode=1;
	}

	glwindow->redraw();
}

   Fl::add_idle(idle);
    return Fl::run();
}

