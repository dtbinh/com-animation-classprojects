/*
      motion.h
 
	  Motion class  

	  1. read an AMC file and store it in a sequence of state vector 
	  2. write an AMC file
	  3. export to a mrdplot format for plotting the trajectories
   
      You can add more motion data processing functions in this class. 

      Revision 1 - Steve Lin, Jan. 14, 2002
 	  Revision 2 - Alla and Kiran, Jan 18, 2002
*/

#ifndef _MOTION_H
#define _MOTION_H

#include "vector.h"
#define	NUM_BONES_IN_ASF_FILE	31
//Root position and all bone rotation angles (including root) 
struct Posture
{
	vector root_pos;
	vector bone_rotation[NUM_BONES_IN_ASF_FILE];
};


class Motion
{
	//member functions 
    public:
		Motion(char *amc_filename, float scale);
       ~Motion();

       // scale is a parameter to adjust the translational parameter
       // This value should be consistent with the scale parameter used in Skeleton()
       // The default value is 0.06
       int readAMCfile(char* name, float scale);
       int writeAMCfile(char* name, float scale);

       // export motion data to mrdplot format
       int writeMRDfile(char *filename);           
       int writeMRDfile(char *filename, float *data, char varNames[][256], char varUnits[][256], int n);


	//data members
	public:
       int m_NumFrames; //Number of frames in the motion 
	   int m_NumDOFs;	//Overall number of degrees of freedom (summation of degrees of freedom for all bones)

	   //Root position and all bone rotation angles for each frame (as read from AMC file)
	   Posture* m_pPostures; 
};

#endif
