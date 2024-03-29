/*
	skeleton.h

	Definition of the skeleton. 

    Revision 1 - Steve Lin, Jan. 14, 2002
 	Revision 2 - Alla and Kiran, Jan 18, 2002
*/

#ifndef _SKELETON_H
#define _SKELETON_H
#define MOCAP_SCALE 0.06
#define	NUM_BONES_IN_ASF_FILE	31

#include "motion.h"

// Bone segment names used in ASF file
static char *AsfPartName[NUM_BONES_IN_ASF_FILE] =
{
	"root",
	"lhipjoint", "lfemur", "ltibia", "lfoot", "ltoes",
	"rhipjoint", "rfemur", "rtibia", "rfoot", "rtoes",
	"lowerback", "upperback", "thorax", "lowerneck", "upperneck", "head",
	"lclavicle", "lhumerus", "lradius", "lwrist", "lhand", "lfingers", "lthumb",
	"rclavicle", "rhumerus", "rradius", "rwrist", "rhand", "rfingers", "rthumb"
};


enum AsfPart	
{
	root=0,
	lhipjoint, lfemur, ltibia, lfoot, ltoes,
	rhipjoint, rfemur, rtibia, rfoot, rtoes,
	lowerback, upperback, thorax, lowerneck, upperneck, head,
	lclavicle, lhumerus, lradius, lwrist, lhand, lfingers, lthumb,
	rclavicle, rhumerus, rradius, rwrist, rhand, rfingers, rthumb
};


// this structure defines the property of each bone segment, including its connection to other bones,
// DOF (degrees of freedom), relative orientation and distance to the outboard bone 
struct Bone {
   
	struct Bone *sibling;		// Pointer to the sibling (branch bone) in the hierarchy tree 
	struct Bone *child;			// Pointer to the child (outboard bone) in the hierarchy tree 
   
	int idx;					// Bone index
	
	float dir[3];				// Unit vector describes the direction from local origin to 
								// the origin of the child bone 
								// Notice: stored in local coordinate system of the bone

	float length;				// Bone length  

	float axis_x, axis_y, axis_z;// orientation of each bone's local coordinate 
								 //system as specified in ASF file (axis field)

	float aspx, aspy;			// aspect ratio of bone shape

	int dof;					// number of bone's degrees of freedom 
	int dofx, dofy, dofz;		// degree of freedom mask in x, y, z axis (local)
								// dofx=1 if this bone has x degree of freedom, otherwise dofx=0.
	
	
	// rotation matrix from the local coordinate of this bone to the local coordinate system of it's parent
	double rot_parent_current[4][4];			
	
	//Rotation angles for this bone at a particular time frame (as read from AMC file) in local coordinate system, 
	//they are set in the setPosture function before dispay function is called
	float drx, dry, drz;		
};


class Skeleton {

  //Member functions
  public: 

	// The scale parameter adjusts the size of the skeleton. The default value is 0.06 (MOCAP_SCALE).
    // This creates a human skeleton of 1.7 m in height (approximately)
    Skeleton(char *asf_filename, float scale);  
    ~Skeleton();                                

    //Get root node's address; for accessing bone data
    Bone* getRoot();

	//Set the skeleton's pose based on the given posture    
	void setPosture(Posture posture);        

	//Initial posture Root at (0,0,0)
	//All bone rotations are set to 0
    void setBasePosture();
	  
    void jointPosition(int boneID, float *pos);
	void local2global(int boneID, float local[3], float global[3]);

  private:

	//parse the skeleton (.ASF) file	
    void readASFfile(char* asf_filename, float scale);


	//This recursive function traverces skeleton hierarchy 
	//and returns a pointer to the bone with index - bIndex
	//ptr should be a pointer to the root node 
	//when this function first called
	Bone* getBone(Bone *ptr, int bIndex);
      

	//This function sets sibling or child for parent bone
	//If parent bone does not have a child, 
	//then pChild is set as parent's child
	//else pChild is set as a sibling of parents already existing child
	int setChildrenAndSibling(int parent, Bone *pChild);

	//Rotate all bone's direction vector (dir) from global to local coordinate system
	void RotateBoneDirToLocalCoordSystem();



  //Member Variables
  public:
	// root position in world coordinate system
    float m_RootPos[3];

    // number of DOF of the skeleton
    int m_NumDOFs;

  private:
	Bone *m_pRootBone;							// Pointer to the root bone, m_RootBone = &bone[0]
	Bone  m_pBoneList[NUM_BONES_IN_ASF_FILE];   // Array with all skeleton bones

};

#endif
