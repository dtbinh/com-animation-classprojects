#include <stdio.h>
#include <stdlib.h>
#include <string.h>	
#include <iostream>
#include <assert.h>
#include <math.h>
#include <FL/gl.h>
#include <GL/glut.h>
#include <FL/fl_file_chooser.H> 
#include "player.h"   		      
#include "interface.h"
#include "transform.h" //utility functions for vector and matrix transformation
#include "display.h"   
#include "Cloth1.h"

/***************  Types *********************/
enum {OFF, ON};


/***************  Static Variables *********/
static Display displayer;		

static Skeleton *pActor = NULL;		// Actor info as read from ASF file
static Motion *pMotion = NULL;		// Motion information as read from AMC file
static CCloth *pCloth = NULL;
static bool bActorExist = false;	// Set to true if actor exists


static int nFrameNum, nFrameInc=1;	// Current frame and frame increment

static Fl_Window *form=NULL;  		// Global form 
static MouseT mouse;				// Keeping track of mouse input 
static CameraT camera;				// Structure about camera setting 

static int Play=OFF, Rewind=OFF;	// Some Flags for player
static int Repeat=OFF, Record=OFF; 

static int Background=ON, Light=OFF;// Flags indicating if the object exists    
static bool Simulation=OFF;			// Flag indicating simulation state

//DEBUG: Remove this variable???
static float root_world[3];			// The character's root position in the world coordinate
static char *Record_filename;		// Recording file name 



/***************  Functions *******************/
static void draw_triad() 
{
    glBegin(GL_LINES);

   /* draw x axis in red, y axis in green, z axis in blue */
   glColor3f(1., .2, .2);
   glVertex3f(0., 0., 0.);
   glVertex3f(1., 0., 0.);

   
   glColor3f(.2, .2, 1.);
   glVertex3f(0., 0., 0.);
   glVertex3f(0., -1., 0.);

   glColor3f(.2, 1., .2);
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
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white1);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
        glColor3f(.6, .6, .6);
      }
      else 
      {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, black);	
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, green5);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, green2);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
        glColor3f(.8, .8, .8);
      }
      
      glNormal3f(0.,0.,1.);
      glVertex3f(i,j,0.);
      glVertex3f(i+1,j,0.);
      glVertex3f(i+1,j+1,0.);
      glVertex3f(i,j+1,0.);
      count++;
    }
  }
  
  glEnd();
}

void cameraView(void)
{
  glTranslated(camera.tx, camera.ty, camera.tz);
  glTranslated(camera.atx, camera.aty, camera.atz);
  
  glRotated(-camera.el, 1.0, 0.0, 0.0);
  glRotated(-camera.tw, 0.0, 0.0, 1.0);
  glRotated(-camera.az, 0.0, 1.0, 0.0); 
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
  glLineWidth(2.);   /* we'll draw background with thick lines */
  glPushMatrix();
  if (Background)
  {
    glRotatef(-90.,1.,0.,0.);
    draw_triad();    /* draw a triad in the origin of the world coord */
    draw_ground();	
  }
  glPopMatrix();

  //if (bActorExist) displayer.show();
  
  // Attach the cloth root to the skeleton root
  // Draw the cloth piece
  if(pCloth)
  {
    pCloth->updateFrameNum(nFrameNum);
    pCloth->drawScene();
  }
  
  glPopMatrix();	    /* restore current transform matrix */

}

/* Callbacks from form. */
void redisplay_proc(Fl_Light_Button *obj, long val) 
{
  Light = light_button->value();
  Background  = background_button->value();
  Simulation = simulate_button->value();
  glwindow->redraw();
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
      if(pActor != NULL)
	  {
			delete pActor;

			//delete old motion if any
			if (pMotion != NULL)
				delete pMotion;			
	  }
      //Read skeleton from asf file
      pActor = new Skeleton(filename, MOCAP_SCALE);
      
      //Set the rotations for all bones in their local coordinate system to 0
      //Set root position to (0, 0, 0)
      pActor->setBasePosture();
      
      displayer.loadActor(pActor);
      bActorExist = true;
    }
  }
  
  if(button==loadCloth_button)
  {
    pCloth = new CCloth();

    if(bActorExist)
    {
      pCloth->LoadActor(pActor);
      pCloth->LoadMotion(pMotion);
      //pCloth->updateSuspensionPoints();
    }
    
	pCloth->CreateClothPatch(pActor->m_RootPos[0]/*-.45f*/,
                            pActor->m_RootPos[1]/*+.15f*/,
                            pActor->m_RootPos[2]/*+.20f*/);
    //pCloth->CreateClothPatch(0.0, 0.0, 0.0);

    pCloth->m_SimRunning = Simulation;
    if (pCloth->m_SimRunning)
      pCloth->m_LastTime = 0;
  }

  if(button==loadMotion_button && bActorExist == true)
  {
    filename = fl_file_chooser("Select filename","*.AMC","");

    if(filename != NULL)
    {
      //delete old motion if any
      if (pMotion != NULL)
	  {
        delete pMotion;
		//pMotion = NULL;
	  }

      
      //Read motion (.amc) file and create a motion
      pMotion = new Motion(filename, MOCAP_SCALE);
      
      //Move actor's root postion to (1, 1, 1) //DEBUG: Why???	
      //root_world[0]=root_world[1]=root_world[2]=1.;
      
      //Set this motion for display
      displayer.loadMotion(pMotion);               
      
      
      //Tell actor to perform the first pose ( first posture )
      pActor->setPosture(displayer.m_pMotion->m_pPostures[0]);          
      
      frame_slider->maximum((double)displayer.m_pMotion->m_NumFrames - 1);
      nFrameNum=0;
	  displayer.m_pActor->setPosture(displayer.m_pMotion->m_pPostures[nFrameNum]);

	  Fl::flush();

    }


  }
  if (button == quickStart_button)
	{
		quickStart();
	}
  glwindow->redraw();
}

void save_callback(Fl_Button *button, void *) 
{
  char *filename;
  if(button==save_button)
    glwindow->save(fl_file_chooser("Save to Jpeg File", "*.jpg", ""));
  if(button==saveAs_button) // save the file into mrdplot format
  {
    filename=(fl_file_chooser("Save to mrd File", "*.mrd", ""));
    pMotion->writeMRDfile(filename);
  }
}

void play_callback(Fl_Button *button, void *)
{
  if(pMotion != NULL)
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
      Record_filename = fl_file_chooser("SaveAnimation to Jpeg Files", "", "");
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
	if (displayer.m_pMotion != NULL)
	{
		if(Rewind==ON)
		{
			nFrameNum=0;
			displayer.m_pActor->setPosture(displayer.m_pMotion->m_pPostures[nFrameNum]);
			Rewind=OFF;
		}

		if(Play==ON) 
		{
			if(nFrameNum >= displayer.m_pMotion->m_NumFrames)  
			{
				if(Repeat == ON)
				{
					nFrameNum=0;
					if(pMotion) displayer.m_pActor->setPosture(pMotion->m_pPostures[nFrameNum]);
				}
				else
				{
					nFrameNum = displayer.m_pMotion->m_NumFrames-1;
					if(pMotion) displayer.m_pActor->setPosture(pMotion->m_pPostures[nFrameNum-1]);
				}
			}
			else
			{
				if(pMotion) displayer.m_pActor->setPosture(pMotion->m_pPostures[nFrameNum]);
			}


			if(Record==ON) 
				glwindow->save(Record_filename);

			if (nFrameNum < displayer.m_pMotion->m_NumFrames)
				nFrameNum += nFrameInc;
		}
	}
	frame_slider->value((double)nFrameNum+1);

	if(pCloth)
	{
		pCloth->m_SimRunning = Simulation;
	}


	glwindow->redraw();
}

void fslider_callback(Fl_Value_Slider *slider, long val)
{
  if(displayer.m_pMotion->m_NumFrames > 0) 
  {
    nFrameNum=(int) frame_slider->value();
    pActor->setPosture(displayer.m_pMotion->m_pPostures[nFrameNum]);
    Fl::flush();
    glwindow->redraw();
  }
}

// locate rotation center at the root
void locate_callback(Fl_Button *obj, void *)
{
  if(bActorExist)
  {
    camera.zoom = 1;
    camera.atx = pMotion->m_pPostures[nFrameNum].root_pos.p[0]; 
    camera.aty = pMotion->m_pPostures[nFrameNum].root_pos.p[1];
    camera.atz = pMotion->m_pPostures[nFrameNum].root_pos.p[2];
  }
  glwindow->redraw();
}

void valueIn_callback(Fl_Value_Input *obj, void *)
{
  displayer.m_SpotJoint = (int) joint_idx->value();
  nFrameInc = (int) fsteps->value();
  glwindow->redraw();
}

void exit_callback(Fl_Button *obj, long val) 
{
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
  
  GLfloat light0_position[] = {1., 1., 5., 0.}; /* directional light (w=0) */
  GLfloat white[] = {1., 1., 1., 1.};
  
  GLfloat light1_position[] = {-3., 1., -1., 0.};
  GLfloat red[] = {1., .3, .3, 1.};
  
  GLfloat light2_position[] = {1., 8., -2., 0.};
  GLfloat blue[] = {.3, .4, 1., 1.};
  
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, black);	/* no ambient */
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
  printf("OpenGL window has %d bits red, %d green, %d blue; viewport is %dx%d\n", red_bits, green_bits, blue_bits, viewport.width, viewport.height);
  
  /* setup perspective camera with OpenGL */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(/*vertical field of view*/ 45.,
                 /*aspect ratio*/ (double) viewport.width/viewport.height,
                 /*znear*/ .1, /*zfar*/ 50.);
  
  /* from here on we're setting modeling transformations */
  glMatrixMode(GL_MODELVIEW);
  glTranslatef(0., -1., -5.);
  camera.zoom = 1;
  camera.tw=0;
  camera.el=-25;
  camera.az=0;
  camera.atx = root_world[0];
  camera.aty = root_world[1];
  camera.atz = root_world[2];
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
    printf("Button Release.  button %d\n", mouse.button);
    mouse.button = 0;
    break;
  case FL_PUSH:
    mouse.x = (Fl::event_x());
    mouse.y = (Fl::event_y());
    mouse.button = (Fl::event_button());
    printf("Mouse click.  button %d at: (%d, %d)\n", mouse.button, 
           mouse.x, mouse.y);
    break;
  case FL_DRAG:
    mouse.x = (Fl::event_x());
    mouse.y = (Fl::event_y());
    delta_x=mouse.x-prev_x;
    delta_y=mouse.y-prev_y; 
    if(mouse.button == 1)
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
        camera.zoom -= (GLdouble) delta_y/100.0;
        if(camera.zoom < 0.) camera.zoom = 0;
      }
    }
    else if(mouse.button==3){
      camera.tx += (GLdouble) delta_x/10.0;
      camera.tz -= (GLdouble) delta_y/10.0; //FLTK's origin is at the left_top corner
    }
    printf("Drag. button %d at: (%d, %d)\n", mouse.button, mouse.x, mouse.y);
    break;
  case FL_KEYBOARD:
    fprintf(stderr, "Key press: %c\n", Fl::event_key());
    printf("Key press: %c\n", Fl::event_key());
    switch (Fl::event_key()) {
    case 'q':
    case 'Q':
    case 65307:
		printf("end!!!\n");
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
    gl_init();
    light_init();    
  }
  
  //Redisplay the screen then put the proper buffer on the screen.
  redisplay();
}

//	Quickly start to load data
void quickStart()
{
// Load actor
	  pActor = new Skeleton("Skeleton.ASF", MOCAP_SCALE);
      
      //Set the rotations for all bones in their local coordinate system to 0
      //Set root position to (0, 0, 0)
      pActor->setBasePosture();
      
      displayer.loadActor(pActor);
      bActorExist = true;
// Load motion
	  //Read motion (.amc) file and create a motion
      pMotion = new Motion("02_02.amc", MOCAP_SCALE);
      
      //Move actor's root postion to (1, 1, 1) //DEBUG: Why???	
      //root_world[0]=root_world[1]=root_world[2]=1.;
      
      //Set this motion for display
      displayer.loadMotion(pMotion);               
      
      
      //Tell actor to perform the first pose ( first posture )
      pActor->setPosture(displayer.m_pMotion->m_pPostures[0]);          
      
      frame_slider->maximum((double)displayer.m_pMotion->m_NumFrames - 1);
      nFrameNum=0;
	  displayer.m_pActor->setPosture(displayer.m_pMotion->m_pPostures[nFrameNum]);

	  Fl::flush();
//	Load cloth
	  pCloth = new CCloth();

    if(bActorExist)
    {
      pCloth->LoadActor(pActor);
      pCloth->LoadMotion(pMotion);
      //pCloth->updateSuspensionPoints();
    }
    
	pCloth->CreateClothPatch(pActor->m_RootPos[0]/*-.45f*/,
                            pActor->m_RootPos[1]/*+.15f*/,
                            pActor->m_RootPos[2]/*+.20f*/);
    //pCloth->CreateClothPatch(0.0, 0.0, 0.0);

    pCloth->m_SimRunning = Simulation;
    if (pCloth->m_SimRunning)
      pCloth->m_LastTime = 0;

	glwindow->redraw();
}


int main(int argc, char **argv) 
{
  
  /* initialize form, sliders and buttons*/
  form = make_window();

  light_button->value(Light);
  background_button->value(Background);
  record_button->value(Record);
  
  frame_slider->value(0);
  
  /*show form, and do initial draw of model */
  form->show();
  glwindow->show(); /* glwindow is initialized when the form is built */
  //parawindow->show();
 
  
  Fl::add_idle(idle);
  return Fl::run();
}
