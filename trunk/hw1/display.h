/*
    display.h

	Display the skeleton, ground plane and other objects.			
 
    Revision 1 - Steve Lin, Jan. 14, 2002
    Revision 2 - Alla and Kiran, Jan 18, 2002
*/

#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <GL/gl.h>
#include "skeleton.h"
#include "motion.h"


class Display 
{

	//member functions
	public: 
		Display();
		~Display();

		//set actor for display
		void loadActor(Skeleton *pActor);
		//set motion for display
		void loadMotion(Motion *pMotion);
      
		//display the scene (actor, ground plane ....)
		void show();
	
	private:
		//Functions for drawing the actor
		void drawBone(Bone *ptr);
		void traverse(Bone *ptr);
   
	
	//member variables	
	public: 
		int m_SpotJoint;		//joint whose local coordinate system is drawn
		Skeleton *m_pActor;		//pointer to current actor
		Motion *m_pMotion;		//pointer to current motion	

    private:  
		GLuint m_BoneList;		//display list with bones
};

#endif
