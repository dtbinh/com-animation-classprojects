///////////////////////////////////////////////////////////////////////////////
//
// Cloth.h : class definition file
//
// Purpose:	Implementation of OpenGL Window of Cloth Animation
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CLOTH_H
#define _CLOTH_H
// Cloth.h : header file

//#include <FL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include "PhysEnv1.h"
#include "motion.h"
#include "skeleton.h"
#include "transform.h"

#define OGL_AXIS_DLIST		50
#define ushort	unsigned short
/// Structure Definitions ///////////////////////////////////////////////////////

struct t_Visual
{
	int		dataFormat;
	float	*vertexData;	// INTERLEAVED VERTEX DATA IN DATAFORMAT
	long	vertexCnt;		// NUMBER OF VERTICES IN VISUAL
	bool	reuseVertices;	// DO I WANT TO USED INDEXED ARRAYS
	ushort	*faceIndex;		// INDEXED VERTEX DATA IF VERTICES ARE REUSED
	int		vSize;			// NUMBER OF FLOATS IN A VERTEX
	long	faceCnt;		// NUMBER OF FACES IN VISUAL
	tVector *faceNormal;	// POINTER TO FACE NORMALS
	long	vPerFace;		// VERTICES PER FACE, EITHER 3 OR 4
	tVector Ka,Kd,Ks;		// COLOR FOR OBJECT
	float	Ns;				// SPECULAR COEFFICIENT
	char    map[255];
	int		glTex;
	tVector bbox[8];		// BBOX COORDS
	tVector transBBox[8];	
};


/////////////////////////////////////////////////////////////////////////////
// CCloth window

class CCloth 
{
// Construction
public:
  CCloth();
  ~CCloth();	
// Attributes
public:
  
  float m_Base_Rot_X,m_Base_Rot_Y,m_Base_Rot_Z;
  float m_Grab_Rot_X,m_Grab_Rot_Y,m_Grab_Rot_Z;
  float m_Grab_Trans_X,m_Grab_Trans_Y,m_Grab_Trans_Z;
  bool  m_DrawGeometry,m_SimRunning;
  int   m_curVisual;
  float m_StartTime;
  int   m_FrameCnt;
  int    m_TimeIterations;   // number of iterations runs between two rendering frames. 
							 // the time interval between two frames is thus 
							 // "m_TimeIterations*m_MaxTimeStep" if fixed time step is used 
  bool  m_UseFixedTimeStep;
  float m_MaxTimeStep;         
  float m_LastTime;
  bool  m_UseStruct,m_UseShear,m_UseBend;
  CPhysEnv m_PhysEnv;
  float* m_ParentPos;
  
  //For Collision detection purposes only. 
  //Make a hierarchy of Collision Cylinders
  Skeleton *m_pActor;		//pointer to current actor
  Motion   *m_pMotion;		//pointer to current motion	
  int	   m_FrameNum,m_FrameNumOld;	//Frame number, to update Collision cylinders
  bool	   m_LoadCyl;		//updates the Collision cylinder list every frame
  tVector  *m_SuspensionPoints;	//Points on the actor's body onto which the cloth is attached
  int	   *m_AttachPts;
  int	   m_AttachPtsCnt; 
  
// Operations
public:
  
  GLvoid  drawScene(GLvoid);									//由外部呼叫的function, 負責畫出衣服, 以及Simulation
  void	NewSystem();
  void	CreateClothPatch(float origx, float origy, float origz);//[Done] 建立衣服的geometry以及連結spring damper
  void	RunSim();												//Clothes Simulation Loop
  void	LoadActor(Skeleton* pSkeleton);							//讀入actor作為碰撞物件
  void	LoadMotion(Motion* pMotion);							//讀入actor動作
  void	CollisionDisplayList(Bone *bone, GLuint *pBoneList);	//Display list of Collision Cylinders
  void	drawCylinder(Bone *ptr,CPhysEnv* p_PhysEnv);			//Displays a Collision Cylinder in the hierarchy
  void	setCollisionCylinders(Bone *ptr,CPhysEnv* p_PhysEnv);	//Sets the Collision Cylinders in the PhysEnv environment.
  void	drawCollisionCylinders();								//Draws the Collision Cylinder Hierarchy
  void	updateFrameNum(int num);								//Gets the current frame number of the animation
  void	updateSuspensionPoints();								//Updates the position of the suspension points as the root moves
  
protected:
  int		m_PickX, m_PickY;
  int		m_ScreenWidth;
  int		m_ScreenHeight;
  GLuint	m_CylinderList;
};

#endif // _CLOTH_H

