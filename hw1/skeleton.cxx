#include <stdio.h>
#include <string.h>
#include <math.h>
#include "skeleton.h"
#include "transform.h"
#include <stdlib.h>
#include <fstream>
#include <iostream>

using namespace std;

/***********************************************************************************************************

   Read skeleton file

***********************************************************************************************************/


// helper function to convert ASF part name into bone index
int name2idx(char *name)
{
	int i=0;
	while(strcmp(AsfPartName[i], name) != 0 && i++ < NUM_BONES_IN_ASF_FILE);
		return(i);
}

void Skeleton::readASFfile(char* asf_filename, float scale)
{
	//open file
	//std::ifstream is(asf_filename, ios::in | ios::nocreate);
    ifstream is(asf_filename, ios::in);
	if (is.fail()) return;

	//
	// ignore header information
	//
	int n;
	char	str[2048], keyword[256];
	while (1)
	{
		is.getline(str, 2048);	
		sscanf(str, "%s", keyword);
		if (strcmp(keyword, ":bonedata") == 0)	break;
	}
	
	//
	// read bone information: global orientation and translation, DOF.
	//
	is.getline(str, 2048);
	char	part[256], *token;
	float length;
	for(int i = 1; i < NUM_BONES_IN_ASF_FILE; i++)
	{		
		m_pBoneList[i].dof=0;
		m_pBoneList[i].dofx=m_pBoneList[i].dofy=m_pBoneList[i].dofz=0;

		while(1)
		{
			is.getline(str, 2048);	sscanf(str, "%s", keyword);

			//name of the bone
			if(strcmp(keyword, "name") == 0)
				sscanf(str, "%s %s", keyword, part);
			
			//this line describes the bone's direction vector in global coordinate
			//it will later be converted to local coorinate system
			if(strcmp(keyword, "direction") == 0)  
				sscanf(str, "%s %f %f %f", keyword, &m_pBoneList[i].dir[0], &m_pBoneList[i].dir[1], &m_pBoneList[i].dir[2]);
			
			//length of the bone
			if(strcmp(keyword, "length") == 0)  
				sscanf(str, "%s %f", keyword, &length);

			//this line describes the orientation of bone's local coordinate 
			//system relative to the world coordinate system
			if(strcmp(keyword, "axis") == 0)      
				sscanf(str, "%s %f %f %f", keyword, &m_pBoneList[i].axis_x, &m_pBoneList[i].axis_y, &m_pBoneList[i].axis_z);

			// this line describes the bone's dof 
			if(strcmp(keyword, "dof") == 0)       
			{
				token=strtok(str, " ");
				m_pBoneList[i].dof=-1;
				while(token != NULL)      
				{
 					if(strcmp(token, "rx") == 0) m_pBoneList[i].dofx = 1;
					if(strcmp(token, "ry") == 0) m_pBoneList[i].dofy = 1;
					if(strcmp(token, "rz") == 0) m_pBoneList[i].dofz = 1;
					token=strtok(NULL, " ");
					m_pBoneList[i].dof++;
				}
				m_NumDOFs+=m_pBoneList[i].dof;
			}
			if(strcmp(keyword, "end") == 0) break;
		}

		//store all the infro we read from the file into the data structure
		m_pBoneList[i].idx = name2idx(part);
		m_pBoneList[i].length = length * scale;
		//init child/sibling to NULL, it will be assigned next (when hierarchy read)
		m_pBoneList[i].sibling = NULL; 
		m_pBoneList[i].child = NULL;
	}

	//
	//read and build the hierarchy of the skeleton
	//
	char *part_name;
	int j, parent;
 
	//find "hierarchy" string in the ASF file
	while(1)
	{
		is.getline(str, 2048);	sscanf(str, "%s", keyword);
		if(strcmp(keyword, ":hierarchy") == 0)	
			break;
	}
	
	//skip "begin" line
	is.getline(str, 2048);

	//Assign parent/child relationship to the bones
	while(1)
	{
		//read next line
		is.getline(str, 2048);	sscanf(str, "%s", keyword);

		//check if we are done
		if(strcmp(keyword, "end") == 0)   
			break;
		else
		{
			//parse this line, it contains parent followed by children
			part_name=strtok(str, " ");
			j=0;
			while(part_name != NULL)
			{
				if(j==0) 
					parent=name2idx(part_name);
				else 
					setChildrenAndSibling(parent, &m_pBoneList[name2idx(part_name)]);
				part_name=strtok(NULL, " ");
				j++;
			}
		}
	}
	is.close();
}


/*
   This recursive function traverces skeleton hierarchy 
   and returns a pointer to the bone with index - bIndex
   ptr should be a pointer to the root node 
   when this function first called
*/
Bone* Skeleton::getBone(Bone *ptr, int bIndex)
{
   static Bone *theptr;
   if(ptr==NULL) 
      return(NULL);
   else if(ptr->idx == bIndex)
   {
      theptr=ptr;
      return(theptr);
   }
   else
   { 
      getBone(ptr->child, bIndex);
      getBone(ptr->sibling, bIndex);
      return(theptr);
   }
}

/*
  This function sets sibling or child for parent bone
  If parent bone does not have a child, 
  then pChild is set as parent's child
  else pChild is set as a sibling of parents already existing child
*/
int Skeleton::setChildrenAndSibling(int parent, Bone *pChild)
{
	Bone *pParent;  
   
	//Get pointer to root bone
	pParent = getBone(m_pRootBone, parent);

	if(pParent==NULL)
	{
		printf("inbord bone is undefined\n"); 
		return(0);
	}
	else
	{
		//if pParent bone does not have a child
		//set pChild as parent bone child
		if(pParent->child == NULL)   
		{
			pParent->child = pChild;
		}
		else
		{ 
			//if pParent bone already has a child 
			//set pChils as pParent bone's child sibling
			pParent=pParent->child;              
			while(pParent->sibling != NULL) 
				pParent = pParent->sibling;            

			pParent->sibling = pChild;
		}
		return(1);
	}
}

/* 
	Return the pointer to the root bone
*/	
Bone* Skeleton::getRoot()
{
   return(m_pRootBone);
}


/***************************************************************************************
  Compute relative orientation and translation between the 
  parent and child bones. That is, represent the orientation 
  matrix and translation vector in the local coordinate of parent body 
*****************************************************************************************/


/*
	This function sets rot_parent_current data member.
	Rotation from this bone local coordinate system 
	to the coordinate system of its parent
*/
void compute_rotation_parent_child(Bone *parent, Bone *child)
{

   double Rx[4][4], Ry[4][4], Rz[4][4], tmp[4][4], tmp1[4][4], tmp2[4][4];

   if(child != NULL)
   { 
     
     // The following openGL rotations are precalculated and saved in the orientation matrix. 
     //
     // glRotatef(-inboard->axis_x, 1., 0., 0.);
     // glRotatef(-inboard->axis_y, 0., 1,  0.);
     // glRotatef(-inboard->axis_z, 0., 0., 1.);
     // glRotatef(outboard->axis_z, 0., 0., 1.);
     // glRotatef(outboard->axis_y, 0., 1,  0.);
     // glRotatef(outboard->axis_x, 1., 0., 0.);

     rotationZ(Rz, -parent->axis_z);      
     rotationY(Ry, -parent->axis_y);  
     rotationX(Rx, -parent->axis_x);      
     matrix_mult(Rx, Ry, tmp);
     matrix_mult(tmp, Rz, tmp1);

     rotationZ(Rz, child->axis_z);
     rotationY(Ry, child->axis_y);
     rotationX(Rx, child->axis_x);
     matrix_mult(Rz, Ry, tmp);
     matrix_mult(tmp, Rx, tmp2);

     matrix_mult(tmp1, tmp2, tmp);
     matrix_transpose(tmp, child->rot_parent_current);    
   }
}


// loop through all bones to calculate local coordinate's direction vector and relative orientation  
void ComputeRotationToParentCoordSystem(Bone *bone)
{
	int i;
	double Rx[4][4], Ry[4][4], Rz[4][4], tmp[4][4], tmp2[4][4];

	//Compute rot_parent_current for the root 

	//Compute tmp2, a matrix containing root 
	//joint local coordinate system orientation
	rotationZ(Rz, bone[root].axis_z);
	rotationY(Ry, bone[root].axis_y);
	rotationX(Rx, bone[root].axis_x);
	matrix_mult(Rz, Ry, tmp);
	matrix_mult(tmp, Rx, tmp2);
	//set bone[root].rot_parent_current to transpose of tmp2
	matrix_transpose(tmp2, bone[root].rot_parent_current);    


	//Compute rot_parent_current for all other bones
	for(i=1; i<NUM_BONES_IN_ASF_FILE; i++) 
	{
		compute_rotation_parent_child(&bone[i], bone[i].child);
	}

	compute_rotation_parent_child(&bone[root], &bone[lhipjoint]);
	compute_rotation_parent_child(&bone[root], &bone[rhipjoint]);
	compute_rotation_parent_child(&bone[root], &bone[lowerback]);
	compute_rotation_parent_child(&bone[thorax], &bone[lclavicle]);
	compute_rotation_parent_child(&bone[thorax], &bone[rclavicle]);
	compute_rotation_parent_child(&bone[lwrist], &bone[lthumb]);
	compute_rotation_parent_child(&bone[rwrist], &bone[rthumb]);
}

/*
	Transform the direction vector (dir), 
	which is defined in character's global coordinate system in the ASF file, 
	to local coordinate
*/
void Skeleton::RotateBoneDirToLocalCoordSystem()
{
	int i;

	for(i=1; i<NUM_BONES_IN_ASF_FILE; i++) 
	{
		//Transform dir vector into local coordinate system
		vector_rotationXYZ(&m_pBoneList[i].dir[0], -m_pBoneList[i].axis_x, -m_pBoneList[i].axis_y, -m_pBoneList[i].axis_z); 
	}

}


/******************************************************************************
Interface functions to set the pose of the skeleton 
******************************************************************************/

//Initial posture Root at (0,0,0)
//All bone rotations are set to 0
void Skeleton::setBasePosture()
{
   int i;
   m_RootPos[0] = m_RootPos[1] = m_RootPos[2] = 0.0;

   for(i=0;i<NUM_BONES_IN_ASF_FILE;i++)
      m_pBoneList[i].drx = m_pBoneList[i].dry = m_pBoneList[i].drz = 0.0;
}


// set the skeleton's pose based on the given posture
void Skeleton::setPosture(Posture posture) 
{
    m_RootPos[0] = posture.root_pos.p[0];
    m_RootPos[1] = posture.root_pos.p[1];
    m_RootPos[2] = posture.root_pos.p[2];

    for(int j=0;j<NUM_BONES_IN_ASF_FILE;j++)
    {
		// if the bone has rotational degree of freedom in x direction
		if(m_pBoneList[j].dofx) 
		   m_pBoneList[j].drx = posture.bone_rotation[j].p[0];  

		// if the bone has rotational degree of freedom in y direction
		if(m_pBoneList[j].dofy) 
		   m_pBoneList[j].dry = posture.bone_rotation[j].p[1];    
		
		// if the bone has rotational degree of freedom in z direction
		if(m_pBoneList[j].dofz) 
		   m_pBoneList[j].drz = posture.bone_rotation[j].p[2];  
    }
}


//Set the aspect ratio of each bone 
void set_bone_shape(Bone *bone)
{
   bone[root].aspx=1;          bone[root].aspy=1;
   bone[lhipjoint].aspx=0.75;   bone[lhipjoint].aspy=0.75;
   bone[lfemur].aspx=0.75;      bone[lfemur].aspy=0.75;
   bone[ltibia].aspx=0.5;     bone[ltibia].aspy=0.5;
   bone[lfoot].aspx=0.25;      bone[lfoot].aspy=0.25;
   bone[ltoes].aspx=0.1;       bone[ltoes].aspy=0.5;
   bone[lclavicle].aspx=0.75;  bone[lclavicle].aspy=0.75;     
   bone[lhumerus].aspx=0.75;    bone[lhumerus].aspy=0.75; 
   bone[lradius].aspx=0.7;     bone[lradius].aspy=0.7;
   bone[lwrist].aspx=0.5;      bone[lwrist].aspy=0.5;
   bone[lhand].aspx=0.2;       bone[lhand].aspy=0.2;     
   bone[lthumb].aspx=0.25;     bone[lthumb].aspy=0.25;     
   bone[lfingers].aspx=0.2;    bone[lfingers].aspy=0.75;     

   bone[lowerback].aspx=2;     bone[lowerback].aspy=1.5;     
   bone[upperback].aspx=2.5;   bone[upperback].aspy=1.5;     
   bone[thorax].aspx=2.5;      bone[thorax].aspy=1.5;     
   bone[lowerneck].aspx=0.4;   bone[lowerneck].aspy=0.5;     
   bone[upperneck].aspx=0.4;   bone[upperneck].aspy=0.5; 
   bone[head].aspx=1.2;        bone[head].aspy=1.2;

   bone[rhipjoint].aspx=0.75;   bone[rhipjoint].aspy=0.75;     
   bone[rfemur].aspx=0.75;      bone[rfemur].aspy=0.75;
   bone[rtibia].aspx=0.5;     bone[rtibia].aspy=0.5;
   bone[rfoot].aspx=0.25;      bone[rfoot].aspy=0.25;
   bone[rtoes].aspx=0.1;       bone[rtoes].aspy=0.5;
   bone[rclavicle].aspx=0.75;  bone[rclavicle].aspy=0.75;     
   bone[rhumerus].aspx=0.75;    bone[rhumerus].aspy=0.75; 
   bone[rradius].aspx=0.7;     bone[rradius].aspy=0.7;
   bone[rwrist].aspx=0.5;      bone[rwrist].aspy=0.5;
   bone[rhand].aspx=0.2;       bone[rhand].aspy=0.8;     
   bone[rthumb].aspx=0.25;     bone[rthumb].aspy=0.25;     
   bone[rfingers].aspx=0.2;    bone[rfingers].aspy=0.75;     
}


// Constructor 
Skeleton::Skeleton(char *asf_filename, float scale)
{

	//Initializaton
	m_pBoneList[0].idx = root;   // root of hierarchy
	m_pRootBone = &m_pBoneList[0];
	m_pBoneList[0].sibling = NULL;
	m_pBoneList[0].child = NULL; 
	m_pBoneList[0].dir[0] = 0; m_pBoneList[0].dir[1] = 0.; m_pBoneList[0].dir[2] = 0.;
	m_pBoneList[0].axis_x = 0; m_pBoneList[0].axis_y = 0.; m_pBoneList[0].axis_z = 0.;
	m_pBoneList[0].length = 0.05;
	m_pBoneList[0].dof = 3;
	m_pBoneList[0].dofx = m_pBoneList[0].dofy = m_pBoneList[0].dofz=1;
	m_RootPos[0] = m_RootPos[1]=m_RootPos[2]=0;
	m_NumDOFs=6;

	// build hierarchy and read in each bone's DOF information
	readASFfile(asf_filename, scale);  

	//transform the direction vector for each bone from the world coordinate system 
	//to it's local coordinate system
	RotateBoneDirToLocalCoordSystem();

	//Calculate rotation from each bone local coordinate system to the coordinate system of its parent
	//store it in rot_parent_current variable for each bone
	ComputeRotationToParentCoordSystem(m_pRootBone);

	//Set the aspect ratio of each bone 
	set_bone_shape(m_pRootBone);
}

Skeleton::~Skeleton()
{
}


/***********************************************************************************************************

   Kinematics calculation functions

***********************************************************************************************************/


// This is a cheating to get a bone's tree path in the hierarchy -- a fixed and known hierarchy in this case
// The function should be fixed to allow different hierarchies.  

void getJointPath(int joint_idx, int *path, int *n)
{
   int i=0, j=0, k;
   bool found=false;
   static int hierarchy[7][11]={
      {root, lhipjoint, lfemur, ltibia, lfoot, ltoes, -1000},
      {root, rhipjoint, rfemur, rtibia, rfoot, rtoes, -1000},
      {root, lowerback, upperback, thorax, lowerneck, upperneck, head, -1000},
      {root, lowerback, upperback, thorax, lclavicle, lhumerus, lradius, lwrist, lhand, lfingers, -1000},
      {root, lowerback, upperback, thorax, lclavicle, lhumerus, lradius, lwrist, lthumb, -1000},
      {root, lowerback, upperback, thorax, rclavicle, rhumerus, rradius, rwrist, rhand, rfingers, -1000},
      {root, lowerback, upperback, thorax, rclavicle, rhumerus, rradius, rwrist, rthumb, -1000}
   };

   *n=0;

   while(i<7 && !found)
   {
      j=0;
      while(j<11 && hierarchy[i][j] != -1000 && !found)
      { 
	 if(hierarchy[i][j] == joint_idx){ 
            found=true;
            for(k=0; k<=j; k++) path[k]=hierarchy[i][k];
	    *n=j+1;    // distal end, if *n=j, near end    
	 }
	 else j++;
      }
      i++;
   }
}

// calculate the joint's global position with respect to the character's coordinate 

void getM_amc(float rx, float ry, float rz, double M[4][4]) 
{
   double Rx[4][4], Ry[4][4], Rz[4][4], tmp[4][4], tmp1[4][4]; 
   rotationX(Rx, rx);
   rotationY(Ry, ry);  
   rotationZ(Rz, rz);      
   matrix_mult(Rz, Ry, tmp);
   matrix_mult(tmp, Rx, M);   
}


// forward kinematics: the position is defined at the distal end of the bone
// Thus, to get the knee joint position, the boneID should be femur
// Note that the lhipjoint/rhipjoint is a dummy joint; 
// Coincidently, to get the lhipjoint/rhipjoint position, 
// the boneID should be hipjoint in this exceptional case

void Skeleton::jointPosition(int boneID, float *pos)
{
   Bone *ptr;
   float local_pos[3];
   int i, j, n;
   int path[31];
   double M[4][4], M_tmp[4][4], M_tmp2[4][4], M_amc[4][4];

   for(i=0; i<3; i++) 
      pos[i] = m_RootPos[i];

   for(i=0; i<4; i++)
   {
      for(j=0; j<4; j++)
         M[i][j]=0;
      M[i][i]=1;
   }
            
   getJointPath(boneID, path, &n);
   for(int b=0;b<n;b++)
   {
      ptr = &m_pBoneList[path[b]];
      
      //  note the relative rotation matrix calculated in coordinate_orientation()
      //  is transposed to match the openGL's transformation matrix format (column-order array)  
      //  Here, we need to transpose it back!
      matrix_transpose(ptr->rot_parent_current, M_tmp);   

      // form rotation matrix in the AMC file 
      getM_amc(ptr->drx, ptr->dry, ptr->drz, M_amc);

      matrix_mult(M, M_tmp, M_tmp2);
      matrix_mult(M_tmp2, M_amc, M);


      //Compute tx, ty, tz - translation from pBone to it's child (in local coordinate system of pBone)
      float tx = ptr->dir[0]*ptr->length;
      float ty = ptr->dir[1]*ptr->length;
      float tz = ptr->dir[2]*ptr->length;
      matrix_transform_affine(M, tx, ty, tz, local_pos);

      for(i=0;i<3;i++) pos[i] += local_pos[i];
   }
}


// get the global coordinates of a point that is represented in the local coordinate
// the local frame is attached at the near end of the joint

void Skeleton::local2global(int boneID, float local[3], float global[3])
{
   Bone *ptr;
   float localVector[3], globalVector[3];
   int i, j, n;
   int path[31];
   double M[4][4], M_tmp[4][4], M_tmp2[4][4], M_amc[4][4];

   for(i=0; i<3; i++) 
      global[i] = m_RootPos[i];

   for(i=0; i<4; i++)
   {
      for(j=0; j<4; j++)
         M[i][j]=0;
      M[i][i]=1;
   }
            
   getJointPath(boneID, path, &n);
   for(int b=0;b<n;b++)
   {
      ptr = &m_pBoneList[path[b]];
      
      //  note the relative rotation matrix calculated in coordinate_orientation()
      //  is transposed to match the openGL's transformation matrix format (column-order array)  
      //  Here, we need to transpose it back!
      matrix_transpose(ptr->rot_parent_current, M_tmp);   

      // form rotation matrix in the AMC file 
      getM_amc(ptr->drx, ptr->dry, ptr->drz, M_amc);

      matrix_mult(M, M_tmp, M_tmp2);
      matrix_mult(M_tmp2, M_amc, M);

	  if(b == n-1) // bone of interest
	  {
		  localVector[0] = local[0];
		  localVector[1] = local[1];
		  localVector[2] = local[2];
	  }
	  else 
	  {
         //Compute tx, ty, tz - translation from pBone to it's child (in local coordinate system of pBone)
          localVector[0] = ptr->dir[0]*ptr->length;
          localVector[1] = ptr->dir[1]*ptr->length;
          localVector[2] = ptr->dir[2]*ptr->length;
	  }

      matrix_transform_affine(M, localVector[0], localVector[1], localVector[2], globalVector);

      for(i=0;i<3;i++) global[i] += globalVector[i];
   }
}
