///////////////////////////////////////////////////////////////////////////////
//
// Cloth.cxx : implementation file
//		CreateClothPatch contains the "MACRO" routine that creates a cloth
//		patch from a mass-spring system
//
///////////////////////////////////////////////////////////////////////////////
#include "debugfile.h"
#include "Cloth1.h"
#include <malloc.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>

DebugFile fdebug("debug.txt");

/// Application Definitions ///////////////////////////////////////////////////
#define ROTATE_SPEED		1.0		// SPEED OF ROTATION
///////////////////////////////////////////////////////////////////////////////
// CCloth

CCloth::CCloth()
{
  // INITIALIZE THE MODE KEYS
  m_DrawGeometry = true;
  m_SimRunning = true;
  m_pActor = NULL;
  m_pMotion = NULL;
  m_SuspensionPoints = NULL;
  m_AttachPts = NULL;
  m_AttachPtsCnt = 0;
  m_UseStruct = true;
  m_UseShear = true;
  m_UseBend = true;
  m_LastTime = 0.0f;
  
  m_TimeIterations = 10;
  m_UseFixedTimeStep = true;
  m_MaxTimeStep = 0.001; // Todo: tune the parameter
  //m_MaxTimeStep = 0.005;
  
  m_FrameCnt = 0;
  m_FrameNum = 0;
  m_FrameNumOld = -1000;
  m_LoadCyl = true;
  m_PickX = -1;
  m_PickY = -1;
  
  glNewList(OGL_AXIS_DLIST,GL_COMPILE);
  glPushMatrix();
  glScalef(4.0,4.0,4.0);
  glBegin(GL_LINES);
  glVertex3f(-0.2f,  0.0f, 0.0f);
  glVertex3f( 0.2f,  0.0f, 0.0f);
  glVertex3f( 0.2f,  0.0f, 0.0f);	// TOP PIECE OF ARROWHEAD
  glVertex3f( 0.15f,  0.04f, 0.0f);
  glVertex3f( 0.2f,  0.0f, 0.0f);	// BOTTOM PIECE OF ARROWHEAD
  glVertex3f( 0.15f, -0.04f, 0.0f);
  glVertex3f( 0.0f,  0.2f, 0.0f);
  glVertex3f( 0.0f, -0.2f, 0.0f);			
  glVertex3f( 0.0f,  0.2f, 0.0f);	// TOP PIECE OF ARROWHEAD
  glVertex3f( 0.04f,  0.15f, 0.0f);
  glVertex3f( 0.0f,  0.2f, 0.0f);	// BOTTOM PIECE OF ARROWHEAD
  glVertex3f( -0.04f,  0.15f, 0.0f);
  glVertex3f( 0.0f,  0.0f,  0.2f);
  glVertex3f( 0.0f,  0.0f, -0.2f);
  glVertex3f( 0.0f,  0.0f, 0.2f);	// TOP PIECE OF ARROWHEAD
  glVertex3f( 0.0f,  0.04f, 0.15f);
  glVertex3f( 0.0f, 0.0f, 0.2f);	// BOTTOM PIECE OF ARROWHEAD
  glVertex3f( 0.0f, -0.04f, 0.15f);
  glEnd();
  glPopMatrix();
  glEndList();
  //m_ParentPos = new float(3);
  m_ParentPos = (float*)malloc(sizeof(float)*3);
}

CCloth::~CCloth()
{
  //DestroySkeleton(&m_Skeleton);
	delete(m_ParentPos);
}

///////////////////////////////////////////////////////////////////////////////
// Procedure:	RunSim
// Purpose:		Actual simulation loop
// Notes:		Allows you to adjust the rate of simulation or to change it
//				to fixed time steps or actual timesteps.
///////////////////////////////////////////////////////////////////////////////		
void CCloth::RunSim()
{
/// Local Variables ///////////////////////////////////////////////////////////
  float Time;
  float DeltaTime = 0.001;
  //m_LastTime = 0;
///////////////////////////////////////////////////////////////////////////////
  
  if (m_UseFixedTimeStep)
  {
    Time = m_LastTime + (m_MaxTimeStep * m_TimeIterations);
  }
  
  if (m_SimRunning)
  {
    while(m_LastTime < Time)
    {
      DeltaTime = Time - m_LastTime;
      
      if(DeltaTime > m_MaxTimeStep)
      {
         DeltaTime = m_MaxTimeStep;
      }

      m_PhysEnv.Simulate(DeltaTime,m_SimRunning);
      m_LastTime += DeltaTime;
    }
	//printf("Time: %f, DeltaTime: %f, last_time: %f\n", Time, DeltaTime, m_LastTime);
    m_LastTime = Time;
  }
  else
  {
    m_PhysEnv.Simulate(DeltaTime,m_SimRunning);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Procedure:	drawScene
// Purpose:		Draws the current OpenGL scene
///////////////////////////////////////////////////////////////////////////////		
GLvoid CCloth::drawScene(GLvoid)
{
  glPushMatrix();

  if(m_FrameNum != m_FrameNumOld) 
  {
    m_LoadCyl = true;
    m_FrameNumOld = m_FrameNum;
  }
  else m_LoadCyl = false;
  
  if(m_LoadCyl)
  {
    if(m_pMotion)
      updateSuspensionPoints();	
  }

  if(m_pMotion)
    drawCollisionCylinders();
  
	RunSim();

	m_PhysEnv.RenderWorld();		// DRAW THE SIMULATION

  glPopMatrix();
  glFinish();
  
  
  m_PickX = -1;
  m_PickY = -1;
}

///////////////////////////////////////////////////////////////////////////////
// Procedure:	NewSystem
// Purpose:		Clears the Simulation
///////////////////////////////////////////////////////////////////////////////		
void CCloth::NewSystem()
{
	m_PhysEnv.FreeSystem();
	m_SimRunning = false;
	drawScene();
}


// debug
inline void print_vertex(int i1, int j1, int i2, int j2)
{
   printf("(%d, %d), (%d, %d)\n", i1, j1, i2, j2); 
}

///////////////////////////////////////////////////////////////////////////////
// Procedure:	CreateClothPatch
// Purpose:		Creates a System to Represent a Cloth Patch
// Arguments:	Number of Segments in U and V to build
///////////////////////////////////////////////////////////////////////////////		
void CCloth::CreateClothPatch(float origx, float origy, float origz) 
{
/// Local Variables ///////////////////////////////////////////////////////////

  t_Visual *visual;
  int		fPos,vPos,l1,l2;
  tTexturedVertex *vertex;
  float	sx,sy,stepx,stepy;
  bool	orientHoriz = true;	//	If the orientation of the cloth is horizontal

  //uv定義patch數量
  int		u = 16, v = 15;  // u: horizontal; v: vertical
 
  float	w = .65f, h = .70f,tsu,tsv,tdu,tdv;
  /*
  float	SstK = 5000.0f,SstD = 60.0f;//Structural Spring
  float	SshK = 5000.0f,SshD = 60.0f;//Shear spring
  float	SflK = 700.0f,SflD = 20.0; //Bend Spring
*/
  float	SstK = 1000.0f,SstD = 5.0f;//Structural Spring
  float	SshK = 1000.0f,SshD = 5.0f;//Shear spring
  float	SflK = 100.0f,SflD = 20.0; //Bend Spring
	//接上人物
	m_AttachPtsCnt = 2;
	if(m_AttachPts) free(m_AttachPts);
	m_AttachPts = (int *)malloc(sizeof(int)*m_AttachPtsCnt);
	if(m_SuspensionPoints) free(m_SuspensionPoints);
	m_SuspensionPoints = (tVector *) malloc( sizeof(tVector)*m_AttachPtsCnt );

	m_AttachPts[0] = 0;
	m_AttachPts[1] = u-1;

	// note that m_PhysEnv.m_pSuspensionPoints is pointed to m_SuspensionPoints
  m_PhysEnv.m_SuspensionPoints = m_SuspensionPoints;
  m_PhysEnv.m_AttachPts = m_AttachPts;

  NewSystem();	// CLEAR WHAT DATA IS THERE
  

  sx = 0; //-(w / 2.0f);
  sy = (0);
  stepx = w / (float) u;
  stepy = -(h / (float)v);
  
  tsu = 0.0f;
  tsv = 0.0f;
  tdu = 1.0f / (float)u;
  tdv = 1.0f / (float)v;
  
    
  //建立mesh
  fPos = (u - 1) * (v - 1) * 2;		// FACE COUNT
  vPos = u * v;
  visual = (t_Visual *)malloc(sizeof(t_Visual));

  visual->reuseVertices = true;
  visual->dataFormat = GL_T2F_V3F;
  visual->vPerFace = 3;
  visual->vSize = 5;					// 3 floats for vertex
  
  visual->vertexData = (float *)malloc(sizeof(float) * visual->vSize * vPos);
  visual->vertexCnt = vPos;
  visual->faceIndex = (unsigned short *)malloc(sizeof(unsigned short) * fPos * visual->vPerFace);
  visual->faceCnt = fPos;


  // SET THE VERTICES
  // FOR MAKING A SKIRT, CONSTRAIN THE FIRST ROW OF POINTS TO A ELLIPSE
	vertex = (tTexturedVertex *)visual->vertexData;
	for (l1 = 0; l1 < v; l1++,tsv+=tdv)
	{
		for (l2 = 0; l2 < u; l2++,tsu+=tdu)
		{
			vertex->u = tsu;
			vertex->v = tsv;
		    
			vertex->x = sx + (stepx * l2)+origx;
			if (orientHoriz)
			{
				vertex->z = sy + (stepy * l1)+origz;
				vertex->y = 0.0f+origy;
			}
			else
			{
				vertex->y = sy + (stepy * l1)+origy;
				vertex->z = 0.0f+origz;
			}
			//Debug info
			//printf("%f, %f, %f\n" ,vertex->x, vertex->y, vertex->z);
			vertex++;
			
		}
	}

  m_PhysEnv.SetClothDimensions(u,v);
  // INFORM THE PHYSICAL SIMULATION OF THE PARTICLES
  m_PhysEnv.SetWorldParticles(
	  (tTexturedVertex *)visual->vertexData,visual->vertexCnt,
	  true, m_AttachPts, m_AttachPtsCnt);
  
  // add your code here, add 3 types of springs

  //	Determine the Ks and Kd of springs
  switch(m_PhysEnv.m_IntegratorType)
  {
  case EULER_INTEGRATOR:
	  SstK = 5000.0f,SstD = 10.0f;//Structural Spring
	  SshK = 5000.0f,SshD = 10.0f;//Shear spring
	  SflK = 1000.0f,SflD = 20.0; //Bend Spring
	  break;
  case MIDPOINT_INTEGRATOR:
	  SstK = 5000.0f,SstD = 60.0f;//Structural Spring
	  SshK = 5000.0f,SshD = 60.0f;//Shear spring
	  SflK = 100.0f,SflD = 20.0; //Bend Spring
	  break;
  case RK4_INTEGRATOR:
	  SstK = 5000.0f,SstD = 60.0f;//Structural Spring
	  SshK = 5000.0f,SshD = 60.0f;//Shear spring
	  SflK = 100.0f,SflD = 20.0; //Bend Spring
	  break;
  case VERLET_INTEGRATOR:
	  SstK = 1000.0f,SstD = 5.0f;//Structural Spring
	  SshK = 1000.0f,SshD = 5.0f;//Shear spring
	  SflK = 100.0f,SflD = 20.0; //Bend Spring
	  break;
  }
  /*
  if (m_PhysEnv.m_IntegratorType == VERLET_INTEGRATOR)
  {
	  SstK = 1000.0f,SstD = 5.0f;//Structural Spring
	  SshK = 1000.0f,SshD = 5.0f;//Shear spring
	  SflK = 100.0f,SflD = 20.0; //Bend Spring
  }
  else
  {
	  SstK = 5000.0f,SstD = 60.0f;//Structural Spring
	  SshK = 5000.0f,SshD = 60.0f;//Shear spring
	  SflK = 100.0f,SflD = 20.0; //Bend Spring
  }
*/
  if (m_UseStruct)
  {
	  int p1, p2, p3, p4;
	  //	Add structural springs
	  for (int i = 0; i < u - 1; i++)
		  for(int j = 0; j < v - 1; j++)
		  {
			  p1 = j * u + i;
			  p2 = p1 + 1;
			  p3 = (j+1)*u+i+1;
			  p4 = p3 - 1;
			  m_PhysEnv.AddSpring(p1, p2, SstK, SstD, STRUCTURAL_SPRING);
			  m_PhysEnv.AddSpring(p2, p3, SstK, SstD, STRUCTURAL_SPRING);
			  m_PhysEnv.AddSpring(p3, p4, SstK, SstD, STRUCTURAL_SPRING);
			  m_PhysEnv.AddSpring(p4, p1, SstK, SstD, STRUCTURAL_SPRING);
		  }
  }
  if (m_UseShear)
  {
	   int p1, p2, p3, p4;
	  //	Add shear springs
	  for (int i = 0; i < u - 1; i++)
		  for(int j = 0; j < v - 1; j++)
		  {
			  p1 = j * u + i;
			  p2 = p1 + 1;
			  p3 = (j+1)*u+i+1;
			  p4 = p3 - 1;
			  m_PhysEnv.AddSpring(p1, p3, SshK, SshD, SHEAR_SPRING);
			  m_PhysEnv.AddSpring(p2, p4, SshK, SshD, SHEAR_SPRING);
		  }
  }
  if (m_UseBend)
  {
	  //	Add bend springs
	  for (int i = 0; i < u ; i++)
		  for(int j = 0; j < v ; j++)
		  {
			  if (i + 2 < u)
				  m_PhysEnv.AddSpring(j * u + i, j * u + i + 2, SflK, SflD, BEND_SPRING);
			  if (j + 2 < v)
				  m_PhysEnv.AddSpring(j * u + i, (j + 2) * u + i , SflK, SflD, BEND_SPRING);
		  }
  } 
  
  //	Add the springs attached to body
  int AttachKs, AttachKd;
  AttachKs = m_PhysEnv.getAttachForceKs();
  AttachKd = m_PhysEnv.getAttachForceKd();
  for (int i = 0; i < m_AttachPtsCnt; i++)
	  m_PhysEnv.AddAttachedSpring(i, AttachKs, AttachKd);

  m_PhysEnv.ResetWorld();
}

void CCloth::LoadActor(Skeleton* pActor)
{
  m_pActor = pActor;
  CollisionDisplayList(m_pActor->getRoot(),&m_CylinderList);
}

void CCloth::LoadMotion(Motion* pMotion)
{
  m_pMotion = pMotion;
}

void CCloth::CollisionDisplayList(Bone *bone, GLuint *pCylList)
{
  int j;
  GLUquadricObj *qobj;
  *pCylList = glGenLists(NUM_BONES_IN_ASF_FILE);
  qobj=gluNewQuadric();
  
  gluQuadricDrawStyle(qobj, (GLenum) GLU_SILHOUETTE);
  gluQuadricNormals(qobj, (GLenum) GLU_SMOOTH);
  for(j=0;j<NUM_BONES_IN_ASF_FILE;j++)
  {
    glNewList(*pCylList + j, GL_COMPILE); 
    float radius = (bone[j].aspx>bone[j].aspy?bone[j].aspy:bone[j].aspx);
    radius *= MOCAP_SCALE;
    gluCylinder(qobj, radius, radius, bone[j].length, 10, 10);
    glEndList(); 
  }
}

void CCloth::drawCylinder(Bone *pBone, CPhysEnv* p_PhysEnv)
{	
  static float z_dir[3] = {0., 0., 1.};
  float r_axis[3], mag, theta;
  
  //Tranform (rotate) from the local coordinate system of this bone to it's parent
  //This step corresponds to doing: ModelviewMatrix = M_k * (rot_parent_current)
  glMultMatrixd((double*)&pBone->rot_parent_current);     
  
  //rotate AMC 
  //This step corresponds to doing: ModelviewMatrix *= R_k+1
  if(pBone->dofz) glRotatef(pBone->drz, 0., 0., 1.);
  if(pBone->dofy) glRotatef(pBone->dry, 0., 1,  0.);
  if(pBone->dofx) glRotatef(pBone->drx, 1., 0., 0.);
  
  glColor3f(1., 1., 0.1);
  
  //Store the current ModelviewMatrix (before adding the translation part)
  glPushMatrix();
  
  //Compute tx, ty, tz - translation from pBone to it's child (in local coordinate system of pBone)
  float tx = pBone->dir[0]*pBone->length;
  float ty = pBone->dir[1]*pBone->length;
  float tz = pBone->dir[2]*pBone->length;
  
  
  // Use the current ModelviewMatrix to display the current bone
  // Rotate the bone from its canonical position (elongated sphere 
  // with its major axis parallel to X axis) to its correct orientation
  tVector com, axis;
  float len,radius;
//	printf("%d\t%d\n",m_LoadCyl,m_SimRunning);
  if(pBone->idx == root)
  {	  
    glCallList(m_CylinderList + pBone->idx);
    //printf("%d\t%d\n",m_LoadCyl,m_SimRunning);
    if(m_LoadCyl)
    {
      if(p_PhysEnv->m_CylCnt>0)
      {
        if(p_PhysEnv->m_Cyl)
          p_PhysEnv->ClearCollisionCyl();
      }
      
	  //printf("debug301\n");	  
      //float* pos = new float[3];
      //float* vec = new float[3];
	  float* pos = (float*)malloc(sizeof(float)*3);
	  float* vec = (float*)malloc(sizeof(float)*3);
	 // printf("debug302\n");
	 
      m_pActor->jointPosition(pBone->idx, pos);
	  
      memcpy(m_ParentPos,pos,sizeof(float)*3);
      
      MAKEVECTOR(com,m_pActor->m_RootPos[0], m_pActor->m_RootPos[1], m_pActor->m_RootPos[2])
        
      vec[0] = 0;
      vec[1] = 0;
      vec[2] = 1;
      vector_rotationXYZ(vec,m_pMotion->m_pPostures[m_FrameNum].bone_rotation[0].p[0],
                         m_pMotion->m_pPostures[m_FrameNum].bone_rotation[0].p[1],
                         m_pMotion->m_pPostures[m_FrameNum].bone_rotation[0].p[2]);
      
      MAKEVECTOR(axis,vec[0],vec[1],vec[2]);
      radius = (pBone->aspx>pBone->aspy?pBone->aspy:pBone->aspx);	  
      radius *= MOCAP_SCALE;
      len = pBone->length;
      p_PhysEnv->AddCollisionCyl(com,len,radius,axis);
      //printf("Root--FrameNum: %d\tCylCount: %d\n",m_FrameNum,p_PhysEnv->m_CylCnt);
      //delete[] pos;
      //delete[] vec;
		
    }
	
  }
  else
  { 	  
    //Compute the angle between the canonical pose and the correct orientation 
    //(specified in pBone->dir) using cross product.
    //Using the formula: r_axis = z_dir x pBone->dir
    
    v3_cross(z_dir, pBone->dir, r_axis);
    theta =  GetAngle(z_dir, pBone->dir, r_axis);
    glRotatef(theta*180./M_PI, r_axis[0], r_axis[1], r_axis[2]);
    glCallList(m_CylinderList + pBone->idx);
    
    if(m_LoadCyl)
    {
		//printf("debug401\n");	  
		float* pos1 = (float*)malloc(sizeof(float)*3);
		float* pos2 = (float*)malloc(sizeof(float)*3);
		//printf("debug402\n");

      m_pActor->jointPosition(pBone->idx, pos1);
      m_pActor->jointPosition((pBone->idx)-1, pos2);
      
      //Hack to traverse the tree cheaply and setting the correct previous junction
      if((pBone->idx==lhipjoint)||(pBone->idx==rhipjoint)||(pBone->idx==lowerback))
        memcpy(pos2,m_ParentPos,sizeof(float)*3);
      if(pBone->idx==thorax)
        memcpy(m_ParentPos,pos1,sizeof(float)*3);
      if((pBone->idx==lclavicle)||(pBone->idx==rclavicle))
        memcpy(pos2,m_ParentPos,sizeof(float)*3);
      
      MAKEVECTOR(com,(pos1[0]+pos2[0])/2,(pos1[1]+pos2[1])/2,(pos1[2]+pos2[2])/2);
      MAKEVECTOR(axis,(pos1[0]-pos2[0]),(pos1[1]-pos2[1]),(pos1[2]-pos2[2]));
      radius = (pBone->aspx>pBone->aspy?pBone->aspy:pBone->aspx);	  
      radius *= MOCAP_SCALE;
      len = pBone->length;

      p_PhysEnv->AddCollisionCyl(com,len,radius,axis);

    }
    
  }
   
  glPopMatrix(); 
  
  // Finally, add the translation component to the ModelviewMatrix
  // This step corresponds to doing: M_k+1 = ModelviewMatrix += T_k+1
  glTranslatef(tx, ty, tz);

}

void CCloth::setCollisionCylinders(Bone *ptr, CPhysEnv* p_PhysEnv)
{
  if(ptr)
  {
    glPushMatrix();
	//printf("debug201 %d\n",(int)(void*)ptr);
    drawCylinder(ptr,p_PhysEnv);

	//printf("debug202 %d\n",(int)(void*)ptr->child);
    setCollisionCylinders(ptr->child, p_PhysEnv);

    glPopMatrix();

	//printf("debug204 %d\n",(int)(void*)ptr->sibling);
    setCollisionCylinders(ptr->sibling, p_PhysEnv);
	//printf("debug204 %d\n",(int)(void*)ptr);
  }
}

void CCloth::drawCollisionCylinders()
{
  if(m_pActor)
  {	  
    glPushMatrix();
    glTranslatef(m_pActor->m_RootPos[0], m_pActor->m_RootPos[1], m_pActor->m_RootPos[2]);
    //draw the cylinders starting from the root
	//printf("debug000\n");
    setCollisionCylinders(m_pActor->getRoot(),&m_PhysEnv);
    glPopMatrix();
  }
  m_LoadCyl = false;
}

void CCloth::updateFrameNum(int num)
{
  m_FrameNum = num;
}

// This function sets the suspension points of the cloth
// In the starter code, these points correspond to the left and right thumb of the hand
void CCloth::updateSuspensionPoints()
{
  float pos1[3];
  float pos2[3];
  float local[3];
  tVector SuspPoint[2];

 
  if(m_AttachPtsCnt > 0)
  {
	
	// suspension points: the edges of the shoulders
	// cylinder radius of upper arm = 0.75*MOCAP_SCALE
	local[0] = 0.75*MOCAP_SCALE;
	local[1] = 0.0;
	local[2] = 0.0;
	m_pActor->local2global(rhumerus, local, pos1); 
	m_pActor->local2global(lhumerus, local, pos2);

	//	Tune the suspension points
	pos1[2] -= 0.1;
	pos2[2] -= 0.1;
	// note that m_PhysEnv.m_pSuspensionPoints is pointed to m_SuspensionPoints		
	memcpy(&m_SuspensionPoints[0], &pos1, sizeof(tVector));
	memcpy(&m_SuspensionPoints[1], &pos2, sizeof(tVector));
  }

#ifdef WRT_DEBUG
  fprintf(fdebug.fp, "CCloth suspend pos = (%f, %f, %f)\n", m_SuspensionPoints[0].x, 
	m_SuspensionPoints[0].y, m_SuspensionPoints[0].z);
  fdebug.flush();
#endif
}





