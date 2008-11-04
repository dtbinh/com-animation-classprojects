///////////////////////////////////////////////////////////////////////////////
//
// PhysEnv.cpp : Physical World implementation file
//
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include "Cloth1.h"
#include "PhysEnv.h"

#include "debugfile.h"
extern DebugFile fdebug;

/////////////////////////////////////////////////////////////////////////////
// CPhysEnv

// INITIALIZE THE SIMULATION WORLD
CPhysEnv::CPhysEnv()
{
	m_IntegratorType = RK4_INTEGRATOR; //EULER_INTEGRATOR; 
	m_u = 9;
	m_v = 9;
	m_Pick[0] = -1;
	m_Pick[1] = -1;
	m_AttachPts[0] = -1;
	m_AttachPts[1] = -1;
	m_ParticleSys[0] = NULL;
	m_ParticleSys[1] = NULL;
	m_ParticleSys[2] = NULL;	// RESET BUFFER
	// THESE TEMP PARTICLE BUFFERS ARE NEEDED FOR THE MIDPOINT AND RK4 INTEGRATOR
	for (int i = 0; i < 5; i++)
		m_TempSys[i] = NULL;
	m_ParticleCnt = 0;
	m_Contact = NULL;
	m_Penetrate = NULL;
	m_Spring = NULL;
	m_SpringCnt = 0;		
	m_MouseForceActive = false;

	m_UseGravity = true;
	m_UseDamping = true;
	m_DrawSprings = true;
	m_DrawStructural = true;	// By default only draw structural springs
	m_DrawBend = false;
	m_DrawShear = true;
	m_DrawVertices	= true;
	m_CollisionActive = true;
	m_CollisionRootFinding = false;		// I AM NOT LOOKING FOR A COLLISION RIGHT AWAY
	m_AttachedtoBody = false;			// CLOTH IS NOT ATTACHED TO ANYTHING
	MAKEVECTOR(m_Gravity, 0.0f, -0.30f, 0.0f)
	m_UserForceMag = 100.0;
	m_UserForceActive = false;
	m_AttachForceKs = 10000.0f; //ATTACH (cloth/body) FORCE CONSTANT
	//m_AttachForceKs = 1000.0f; //ATTACH (cloth/body) FORCE CONSTANT
	m_AttachForceKd = 1.5f;
	m_MouseForceKs = 2.0f;	// MOUSE SPRING CONSTANT
	m_PenaltyKs = 50000.0f;
	m_Kd	= 0.04f;	// DAMPING FACTOR
	m_Kr	= 0.2f;		// 1.0 = SUPERBALL BOUNCE 0.0 = DEAD WEIGHT
	m_Ksh	= 5.0f;		// HOOK'S SPRING CONSTANT
	m_Ksd	= 0.1f;		// SPRING DAMPING CONSTANT

	// CREATE THE SIZE FOR THE SIMULATION WORLD
	m_WorldSizeX = 15.0f;
	m_WorldSizeY = 1.0f;
	m_WorldSizeZ = 15.0f;

	m_CollisionPlane = (tCollisionPlane	*)malloc(sizeof(tCollisionPlane) * 6);
	m_CollisionPlaneCnt = 6;

	// MAKE THE TOP PLANE (CEILING)
	MAKEVECTOR(m_CollisionPlane[0].normal,0.0f, -1.0f, 0.0f)
	m_CollisionPlane[0].d = 3.0f; // m_WorldSizeY / 2.0f;

	// MAKE THE BOTTOM PLANE (FLOOR)
	MAKEVECTOR(m_CollisionPlane[1].normal,0.0f, 1.0f, 0.0f)
	m_CollisionPlane[1].d = 0; //m_WorldSizeY / 2.0f;

	// MAKE THE LEFT PLANE
	MAKEVECTOR(m_CollisionPlane[2].normal,-1.0f, 0.0f, 0.0f)
	m_CollisionPlane[2].d = m_WorldSizeX / 2.0f;

	// MAKE THE RIGHT PLANE
	MAKEVECTOR(m_CollisionPlane[3].normal,1.0f, 0.0f, 0.0f)
	m_CollisionPlane[3].d = m_WorldSizeX / 2.0f;

	// MAKE THE FRONT PLANE
	MAKEVECTOR(m_CollisionPlane[4].normal,0.0f, 0.0f, -1.0f)
	m_CollisionPlane[4].d = m_WorldSizeZ / 2.0f;

	// MAKE THE BACK PLANE
	MAKEVECTOR(m_CollisionPlane[5].normal,0.0f, 0.0f, 1.0f)
	m_CollisionPlane[5].d = m_WorldSizeZ / 2.0f;

	m_SphereCnt = 0;
	m_CylCnt = 0;
	m_Cyl = (tCollisionCyl*)malloc(sizeof(tCollisionCyl) * (NUM_BONES_IN_ASF_FILE));
}

CPhysEnv::~CPhysEnv()
{
	if (m_ParticleSys[0])
		free(m_ParticleSys[0]);
	if (m_ParticleSys[1])
		free(m_ParticleSys[1]);
	if (m_ParticleSys[2])
		free(m_ParticleSys[2]);
	for (int i = 0; i < 5; i++)
	{
		if (m_TempSys[i])
			free(m_TempSys[i]);
	}
	if (m_Contact)
		free(m_Contact);
	if (m_Penetrate)
		free(m_Penetrate);
	free(m_CollisionPlane);
	free(m_Spring);

	free(m_Sphere);
	free(m_Cyl);
}

void CPhysEnv::SetClothDimensions(int u, int v)
{
	m_u = u;
	m_v = v;
}

void CPhysEnv::RenderWorld()
{
	tParticle	*tempParticle;
	tSpring		*tempSpring;
    
        // draw floor
        glDisable(GL_CULL_FACE);
        glBegin(GL_QUADS);
        glColor3f(0.0f,0.0f,0.5f);
        glVertex3f(-m_WorldSizeX/2.0f,-m_WorldSizeY/2.0f,-m_WorldSizeZ/2.0f);
        glVertex3f( m_WorldSizeX/2.0f,-m_WorldSizeY/2.0f,-m_WorldSizeZ/2.0f);
        glVertex3f( m_WorldSizeX/2.0f,-m_WorldSizeY/2.0f, m_WorldSizeZ/2.0f);
        glVertex3f(-m_WorldSizeX/2.0f,-m_WorldSizeY/2.0f, m_WorldSizeZ/2.0f);
        glEnd();
        glEnable(GL_CULL_FACE);


	if (m_ParticleSys)
	{
          if (m_Spring && m_DrawSprings)
          {
            glBegin(GL_LINES);
            glColor3f(0.0f,0.8f,0.8f);
            tempSpring = m_Spring;
            for (int loop = 0; loop < m_SpringCnt; loop++)
            {
	     // Only draw normal springs or the cloth "structural" ones
              if ((tempSpring->type == MANUAL_SPRING) ||
                  (tempSpring->type == STRUCTURAL_SPRING && m_DrawStructural) ||
                  (tempSpring->type == SHEAR_SPRING && m_DrawShear) ||
                  (tempSpring->type == BEND_SPRING && m_DrawBend))
              {
                glVertex3fv((float *)&m_CurrentSys[tempSpring->p1].pos);
                glVertex3fv((float *)&m_CurrentSys[tempSpring->p2].pos);
              }
              tempSpring++;
            }
            if (m_MouseForceActive)	// DRAW MOUSESPRING FORCE
            {
              if (m_Pick[0] > -1)
              {
                glColor3f(0.8f,0.0f,0.8f);
                glVertex3fv((float *)&m_CurrentSys[m_Pick[0]].pos);
                glVertex3fv((float *)&m_MouseDragPos[0]);
              }
              if (m_Pick[1] > -1)
              {
                glColor3f(0.8f,0.0f,0.8f);
                glVertex3fv((float *)&m_CurrentSys[m_Pick[1]].pos);
                glVertex3fv((float *)&m_MouseDragPos[1]);
              }
            }
            glEnd();
            if (m_AttachedtoBody)	// DRAW MOUSESPRING FORCE
            {
              if (m_AttachPts[0] > -1)
              {
                glColor3f(0.8f,0.0f,0.8f);
                glVertex3fv((float *)&m_CurrentSys[m_AttachPts[0]].pos);
                glVertex3fv((float *)&m_SuspensionPoints[0]);
              }
              if (m_AttachPts[1] > -1)
              {
                glColor3f(0.8f,0.0f,0.8f);
                glVertex3fv((float *)&m_CurrentSys[m_AttachPts[1]].pos);
                glVertex3fv((float *)&m_SuspensionPoints[1]);
              }
            }
            glEnd();
          }
          
          
          if (m_DrawVertices)
          {
            tVector* temp_array;
            temp_array = (tVector*)malloc(m_ParticleCnt*sizeof(tVector));
            tempParticle = m_CurrentSys;
            int loop;
            
            for (loop = 0; loop < m_ParticleCnt; loop++)
            {
              memcpy(&temp_array[loop],&tempParticle->pos,sizeof(tVector));
              tempParticle++;
            }			
            
            if(loop>0)
            {
              glBegin(GL_QUAD_STRIP);
              for (int i=0; i<m_u-1; i++)
                for(int j=0; j<m_v-1; j++)
                {
                  glVertex3fv((float *)&temp_array[j*m_u+i]);
                  glVertex3fv((float *)&temp_array[j*m_u+i+1]);
                  glVertex3fv((float *)&temp_array[(j+1)*m_u+i]);
                  glVertex3fv((float *)&temp_array[(j+1)*m_u+i+1]);					
                }
              glEnd();
            }
            free(temp_array);
          }
#if 0
          if (m_DrawVertices)
          {
            glPointSize(5);
            glBegin(GL_POINTS);
            
            tempParticle = m_CurrentSys;
            
            for (int loop = 0; loop < m_ParticleCnt; loop++)
            {
              if (loop == m_Pick[0])
                glColor3f(0.0f,0.8f,0.0f);
              else if (loop == m_Pick[1])
                glColor3f(0.8f,0.0f,0.0f);
              else
                glColor3f(0.8f,0.8f,0.0f);
              glVertex3fv((float *)&tempParticle->pos);
              tempParticle++;
            }
            glEnd();
          }
#endif
          
	}

	if (m_SphereCnt > 0 && m_CollisionActive)
	{
          glColor3f(0.5f,0.0f,0.0f);
          for (int loop = 0; loop < m_SphereCnt; loop++)
          {
            glPushMatrix();
            glTranslatef(m_Sphere[loop].pos.x, m_Sphere[loop].pos.y, m_Sphere[loop].pos.z);
            glScalef(m_Sphere[loop].radius,m_Sphere[loop].radius,m_Sphere[loop].radius);
            glCallList(OGL_AXIS_DLIST);
            glPopMatrix();
          }
	}
        
	if (m_CylCnt > 0 && m_CollisionActive)
	{
          float z_dir[3] = {0., 0., 1.};
          float cyl_dir[3],r_axis[3];
          
          // Draw scaled arrows showing the location of the cylinders
          glColor3f(0.5f,0.0f,0.0f);
          for (int loop = 0; loop < m_CylCnt; loop++)
          {
            glPushMatrix();
            
            cyl_dir[0] = m_Cyl[loop].axis.x;
            cyl_dir[1] = m_Cyl[loop].axis.y;
            cyl_dir[2] = m_Cyl[loop].axis.z;
        
            v3_cross(z_dir, cyl_dir, r_axis);
            float theta =  GetAngle(z_dir, cyl_dir, r_axis);
            glTranslatef(m_Cyl[loop].pos.x, m_Cyl[loop].pos.y, m_Cyl[loop].pos.z);
            glRotatef(theta*180./M_PI, r_axis[0], r_axis[1], r_axis[2]);
            glScalef(m_Cyl[loop].radius,m_Cyl[loop].radius,m_Cyl[loop].len);
            glCallList(OGL_AXIS_DLIST);
            glPopMatrix();
          }
	}                
}

void CPhysEnv::SetWorldParticles(tTexturedVertex *coords,int particleCnt, bool isAttached, int pt1, int pt2)
{
	tParticle *tempParticle;

	if (m_ParticleSys[0])
		free(m_ParticleSys[0]);
	if (m_ParticleSys[1])
		free(m_ParticleSys[1]);
	if (m_ParticleSys[2])
		free(m_ParticleSys[2]);
	for (int i = 0; i < 5; i++)
	{
		if (m_TempSys[i])
			free(m_TempSys[i]);
	}
	if (m_Contact)
		free(m_Contact);
	// THE SYSTEM IS DOUBLE BUFFERED TO MAKE THINGS EASIER
	m_CurrentSys = (tParticle *)malloc(sizeof(tParticle) * particleCnt);
	m_TargetSys = (tParticle *)malloc(sizeof(tParticle) * particleCnt);
	m_ParticleSys[2] = (tParticle *)malloc(sizeof(tParticle) * particleCnt);
	for (i = 0; i < 5; i++)
	{
		m_TempSys[i] = (tParticle *)malloc(sizeof(tParticle) * particleCnt);
	}
	m_ParticleCnt = particleCnt;

	// MULTIPLIED PARTICLE COUNT * 5
	// the particles can collide with more than one surface (5, for instance)
	m_Contact = (tContact *)malloc(sizeof(tContact) * particleCnt * 5);
	m_ContactCnt = 0;

	m_Penetrate = (tPenetrate *)malloc(sizeof(tPenetrate) * particleCnt * 5);
	m_PenetrateCnt = 0;

	tempParticle = m_CurrentSys;
	for (int loop = 0; loop < particleCnt; loop++)
	{
		MAKEVECTOR(tempParticle->pos, coords->x, coords->y, coords->z)
		MAKEVECTOR(tempParticle->v, 0.0f, 0.0f, 0.0f)
		MAKEVECTOR(tempParticle->f, 0.0f, 0.0f, 0.0f)
		tempParticle->oneOverM = 1.0f;		// MASS OF 1
		tempParticle++;
		coords++;
	}

	// COPY THE SYSTEM TO THE SECOND ONE ALSO
	memcpy(m_TargetSys,m_CurrentSys,sizeof(tParticle) * particleCnt);
	// COPY THE SYSTEM TO THE RESET BUFFER ALSO
	memcpy(m_ParticleSys[2],m_CurrentSys,sizeof(tParticle) * particleCnt);

	m_ParticleSys[0] = m_CurrentSys;
	m_ParticleSys[1] = m_TargetSys;
	
	// INDEX OF THE ATTACHED POINT
	if(isAttached)
	{
		m_AttachedtoBody = true;
		m_AttachPts[0] = pt1;
		m_AttachPts[1] = pt2;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Function:	FreeSystem
// Purpose:		Remove all particles and clear it out
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::FreeSystem()
{
	m_Pick[0] = -1;
	m_Pick[1] = -1;
	if (m_ParticleSys[0])
	{
		m_ParticleSys[0] = NULL;
		free(m_ParticleSys[0]);
	}
	if (m_ParticleSys[1])
	{
		free(m_ParticleSys[1]);
		m_ParticleSys[1] = NULL;
	}
	if (m_ParticleSys[2])
	{
		free(m_ParticleSys[2]);
		m_ParticleSys[2] = NULL;	// RESET BUFFER
	}
	for (int i = 0; i < 5; i++)
	{
		if (m_TempSys[i])
		{
			free(m_TempSys[i]);
			m_TempSys[i] = NULL;	// RESET BUFFER
		}
	}
	if (m_Contact)
	{
		free(m_Contact);
		m_Contact = NULL;
	}
	if (m_Spring)
	{
		free(m_Spring);
		m_Spring = NULL;
	}
	m_SpringCnt = 0;	
	m_ParticleCnt = 0;
}
////// FreeSystem //////////////////////////////////////////////////////////////

void CPhysEnv::LoadData(FILE *fp)
{
	fread(&m_UseGravity,sizeof(bool),1,fp);
	fread(&m_UseDamping,sizeof(bool),1,fp);
	fread(&m_UserForceActive,sizeof(bool),1,fp);
	fread(&m_Gravity,sizeof(tVector),1,fp);
	fread(&m_UserForce,sizeof(tVector),1,fp);
	fread(&m_UserForceMag,sizeof(float),1,fp);
	fread(&m_Kd,sizeof(float),1,fp);
	fread(&m_Kr,sizeof(float),1,fp);
	fread(&m_Ksh,sizeof(float),1,fp);
	fread(&m_Ksd,sizeof(float),1,fp);
	fread(&m_ParticleCnt,sizeof(int),1,fp);
	m_CurrentSys = (tParticle *)malloc(sizeof(tParticle) * m_ParticleCnt);
	m_TargetSys = (tParticle *)malloc(sizeof(tParticle) * m_ParticleCnt);
	m_ParticleSys[2] = (tParticle *)malloc(sizeof(tParticle) * m_ParticleCnt);
	for (int i = 0; i < 5; i++)
	{
		m_TempSys[i] = (tParticle *)malloc(sizeof(tParticle) * m_ParticleCnt);
	}
	m_ParticleSys[0] = m_CurrentSys;
	m_ParticleSys[1] = m_TargetSys;
	m_Contact = (tContact *)malloc(sizeof(tContact) * m_ParticleCnt);
	fread(m_ParticleSys[0],sizeof(tParticle),m_ParticleCnt,fp);
	fread(m_ParticleSys[1],sizeof(tParticle),m_ParticleCnt,fp);
	fread(m_ParticleSys[2],sizeof(tParticle),m_ParticleCnt,fp);
	fread(&m_SpringCnt,sizeof(int),1,fp);
	m_Spring = (tSpring *)malloc(sizeof(tSpring) * (m_SpringCnt));
	fread(m_Spring,sizeof(tSpring),m_SpringCnt,fp);
	fread(m_Pick,sizeof(int),2,fp);
	fread(&m_SphereCnt,sizeof(int),1,fp);
	m_Sphere = (tCollisionSphere *)malloc(sizeof(tCollisionSphere) * (m_SphereCnt));
	fread(m_Sphere,sizeof(tCollisionSphere),m_SphereCnt,fp);
}

void CPhysEnv::SaveData(FILE *fp)
{
	fwrite(&m_UseGravity,sizeof(bool),1,fp);
	fwrite(&m_UseDamping,sizeof(bool),1,fp);
	fwrite(&m_UserForceActive,sizeof(bool),1,fp);
	fwrite(&m_Gravity,sizeof(tVector),1,fp);
	fwrite(&m_UserForce,sizeof(tVector),1,fp);
	fwrite(&m_UserForceMag,sizeof(float),1,fp);
	fwrite(&m_Kd,sizeof(float),1,fp);
	fwrite(&m_Kr,sizeof(float),1,fp);
	fwrite(&m_Ksh,sizeof(float),1,fp);
	fwrite(&m_Ksd,sizeof(float),1,fp);
	fwrite(&m_ParticleCnt,sizeof(int),1,fp);
	fwrite(m_ParticleSys[0],sizeof(tParticle),m_ParticleCnt,fp);
	fwrite(m_ParticleSys[1],sizeof(tParticle),m_ParticleCnt,fp);
	fwrite(m_ParticleSys[2],sizeof(tParticle),m_ParticleCnt,fp);
	fwrite(&m_SpringCnt,sizeof(int),1,fp);
	fwrite(m_Spring,sizeof(tSpring),m_SpringCnt,fp);
	fwrite(m_Pick,sizeof(int),2,fp);
	fwrite(&m_SphereCnt,sizeof(int),1,fp);
	fwrite(m_Sphere,sizeof(tCollisionSphere),m_SphereCnt,fp);
}

// RESET THE SIM TO INITIAL VALUES
void CPhysEnv::ResetWorld()
{
	memcpy(m_CurrentSys,m_ParticleSys[2],sizeof(tParticle) * m_ParticleCnt);
	memcpy(m_TargetSys,m_ParticleSys[2],sizeof(tParticle) * m_ParticleCnt);

/*
	for(int i=0; i<m_v; i++)
		for(int j=0; j<m_u; j++)
		{
			m_CurrentSys[i*m_u+j].pos.z = -m_CurrentSys[i*m_u+j].pos.y;
			m_CurrentSys[i*m_u+j].pos.y = 1.5;
		}
*/	
    // set initialization condition of particle position
	if(m_AttachedtoBody)
	{
		if(m_AttachPts[0] > -1)
		{
			m_CurrentSys[m_AttachPts[0]].pos.x = m_SuspensionPoints[0].x;
			m_CurrentSys[m_AttachPts[0]].pos.y = m_SuspensionPoints[0].y;
			m_CurrentSys[m_AttachPts[0]].pos.z = m_SuspensionPoints[0].z;
		}
		if(m_AttachPts[1] > -1)
		{
			m_CurrentSys[m_AttachPts[1]].pos.x = m_SuspensionPoints[1].x;
			m_CurrentSys[m_AttachPts[1]].pos.y = m_SuspensionPoints[1].y;
			m_CurrentSys[m_AttachPts[1]].pos.z = m_SuspensionPoints[1].z;
		}

		float dx0, dy0, dz0, dx;
		dx0 = (m_SuspensionPoints[1].x - m_SuspensionPoints[0].x) / (float)(m_u-1);
		dy0 = (m_SuspensionPoints[1].y - m_SuspensionPoints[0].y) / (float)(m_u-1);
		dz0 = (m_SuspensionPoints[1].z - m_SuspensionPoints[0].z) / (float)(m_u-1);

		// initialize the particle position at the first row by interpolation the suspension positions
		for(int j=0; j<m_u; j++)
		{
			m_CurrentSys[j].pos.x = m_SuspensionPoints[0].x + dx0*j;
			m_CurrentSys[j].pos.y = m_SuspensionPoints[0].y + dy0*j;
			m_CurrentSys[j].pos.z = m_SuspensionPoints[0].z + dz0*j;
		}

		dx = -0.8 / (float)(m_v - 1); 
		for(int i=1; i<m_v; i++)	
			for(int j=0; j<m_u; j++)
			{
				m_CurrentSys[i*m_u+j].pos.x = m_CurrentSys[(i-1)*m_u+j].pos.x + dx;
				m_CurrentSys[i*m_u+j].pos.y = m_CurrentSys[(i-1)*m_u+j].pos.y;
				m_CurrentSys[i*m_u+j].pos.z = m_CurrentSys[(i-1)*m_u+j].pos.z;
			}
	}
}

void CPhysEnv::SetWorldProperties()
{
// You can set the world parameters (spring constants, mass, dampers, etc) from the GUI
}

// The current implementation does not support user forces (from UI).
// Contact me if you want this enabled
void CPhysEnv::ApplyUserForce(tVector *force)
{
	ScaleVector(force,  m_UserForceMag, &m_UserForce);
	m_UserForceActive = true;
}

///////////////////////////////////////////////////////////////////////////////
// Function:	SetMouseForce 
// Purpose:		Allows the user to interact with selected points by dragging
// Arguments:	Delta distance from clicked point, local x and y axes
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::SetMouseForce(int deltaX,int deltaY, tVector *localX, tVector *localY)
{
/// Local Variables ///////////////////////////////////////////////////////////
	tVector tempX,tempY;
///////////////////////////////////////////////////////////////////////////////
	ScaleVector(localX,  (float)deltaX * 0.03f, &tempX);
	ScaleVector(localY,  -(float)deltaY * 0.03f, &tempY);
	if (m_Pick[0] > -1)
	{
		VectorSum(&m_CurrentSys[m_Pick[0]].pos,&tempX,&m_MouseDragPos[0]);
		VectorSum(&m_MouseDragPos[0],&tempY,&m_MouseDragPos[0]);
	}
	if (m_Pick[1] > -1)
	{
		VectorSum(&m_CurrentSys[m_Pick[1]].pos,&tempX,&m_MouseDragPos[1]);
		VectorSum(&m_MouseDragPos[1],&tempY,&m_MouseDragPos[1]);
	}
}
/// SetMouseForce /////////////////////////////////////////////////////////////


void CPhysEnv::AddSpring()
{
	tSpring	*spring;
	// MAKE SURE TWO PARTICLES ARE PICKED
	if (m_Pick[0] > -1 && m_Pick[1] > -1)
	{
		spring = (tSpring *)malloc(sizeof(tSpring) * (m_SpringCnt + 1));
		if (m_Spring != NULL)
			memcpy(spring,m_Spring,sizeof(tSpring) * m_SpringCnt);
		m_Spring = spring;
		spring = &m_Spring[m_SpringCnt++];
		spring->Ks = m_Ksh;
		spring->Kd = m_Ksd;
		spring->p1 = m_Pick[0];
		spring->p2 = m_Pick[1];
		spring->restLen = 
			sqrt(VectorSquaredDistance(&m_CurrentSys[m_Pick[0]].pos, 
									   &m_CurrentSys[m_Pick[1]].pos));
		spring->type = 	MANUAL_SPRING;
	}
}

void CPhysEnv::AddSpring(int v1, int v2,float Ksh,float Ksd, int type)
{
	tSpring	*spring;
	// MAKE SURE TWO PARTICLES ARE PICKED
	if (v1 > -1 && v2 > -1)
	{
		spring = (tSpring *)malloc(sizeof(tSpring) * (m_SpringCnt + 1));
		if (m_Spring != NULL)
		{
			memcpy(spring,m_Spring,sizeof(tSpring) * m_SpringCnt);
			free(m_Spring);
		}
		m_Spring = spring;
		spring = &m_Spring[m_SpringCnt++];
		spring->type = type;
		spring->Ks = Ksh;
		spring->Kd = Ksd;
		spring->p1 = v1;
		spring->p2 = v2;
		spring->restLen = 
			sqrt(VectorSquaredDistance(&m_CurrentSys[v1].pos, 
									   &m_CurrentSys[v2].pos));
	}
}

// This is the main function that computes the net forces at every timestep of the simulation
// 
// 15497: WRITE CODE HERE
void CPhysEnv::ComputeForces( tParticle	*system )
{
  int loop;
  tParticle	*curParticle,*p1, *p2;
  tSpring		*spring;
  float		dist, Hterm, Dterm;
  tVector		springForce, deltaV, deltaP, dampForce;
  
  curParticle = system;
  //Add gravity to all points
  for (loop = 0; loop < m_ParticleCnt; loop++)
  {
      MAKEVECTOR(curParticle->f,0.0f,0.0f,0.0f)		// CLEAR FORCE VECTOR
  
      if (m_UseGravity && curParticle->oneOverM != 0)
      {
        curParticle->f.x += (m_Gravity.x / curParticle->oneOverM);
        curParticle->f.y += (m_Gravity.y / curParticle->oneOverM);
        curParticle->f.z += (m_Gravity.z / curParticle->oneOverM);
      }

	  if(m_UseDamping)
	  {
		ScaleVector(&curParticle->v, -m_Kd, &dampForce);
		VectorSum(&curParticle->f, &dampForce, &curParticle->f);
	  }

    curParticle++;
  }
 
  spring = m_Spring;
  //Add structure spring-damper force 
  for (loop = 0; loop < m_SpringCnt; loop++)
  {
      if (m_Spring->type == STRUCTURAL_SPRING)
      {
		 p1 = &system[spring->p1];
		 p2 = &system[spring->p2];
         VectorDifference(&p1->pos, &p2->pos, &deltaP);	// Vector distance
		 VectorDifference(&p1->v, &p2->v, &deltaV);		// Vector distance
         dist = VectorLength(&deltaP);					// current length

		 if (dist != 0.0f)
		 {
			Hterm = (dist - spring->restLen) * spring->Ks;				// Ks * dist			
			Dterm = (DotProduct(&deltaV,&deltaP) * spring->Kd) / dist; 
			
			ScaleVector(&deltaP, 1.0f / dist, &springForce);			// Normalize Distance Vector
			ScaleVector(&springForce, -(Hterm+Dterm), &springForce);	// Calc Force
			VectorSum(&p1->f, &springForce, &p1->f);					// Apply to Particle 1
			VectorDifference(&p2->f,&springForce, &p2->f);				// Apply to Particle 2
		 }
      }

	  spring++;
  }

  //Constrain two points in the cloth to be attached to the body by springs
  if (m_AttachedtoBody)
  {
    if (m_AttachPts[0] > -1)
    {
      p1 = &system[m_AttachPts[0]];
      VectorDifference(&p1->pos,&m_SuspensionPoints[0],&deltaP);	// Vector distance 
      dist = VectorLength(&deltaP);					// Magnitude of deltaP
      
      if (dist != 0.0f)
      {
        Hterm = (dist) * m_AttachForceKs;				// Ks * dist
        Dterm = (DotProduct(&deltaP,&p1->v) * m_AttachForceKd) / dist; // Damping Term
        
        ScaleVector(&deltaP,1.0f / dist, &springForce);	// Normalize Distance Vector
        ScaleVector(&springForce,-(Hterm+Dterm),&springForce);	// Calc Force
        VectorSum(&p1->f,&springForce,&p1->f);			// Apply to Particle 1
      }
    }
    if (m_AttachPts[1] > -1)
    {
      p1 = &system[m_AttachPts[1]];
      VectorDifference(&p1->pos,&m_SuspensionPoints[1],&deltaP);	// Vector distance 
      dist = VectorLength(&deltaP);					// Magnitude of deltaP
      
      if (dist != 0.0f)
      {
        Hterm = (dist) * m_AttachForceKs;		        	// Ks * dist
        Dterm = (DotProduct(&deltaP,&p1->v) * m_AttachForceKd) / dist; // Damping Term
        
        ScaleVector(&deltaP,1.0f / dist, &springForce);	// Normalize Distance Vector
        ScaleVector(&springForce,-(Hterm+Dterm),&springForce);	// Calc Force
        VectorSum(&p1->f,&springForce,&p1->f);			// Apply to Particle 1
      }
    }
  }



}   

///////////////////////////////////////////////////////////////////////////////
// Function:	IntegrateSysOverTime 
// Purpose:		Does the Integration for all the points in a system
// Arguments:	Initial Position, Source and Target Particle Systems and Time
// Notes:		Computes a single integration step
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::IntegrateSysOverTime(tParticle *initial,tParticle *source, tParticle *target, float deltaTime)
{
/// Local Variables ///////////////////////////////////////////////////////////
  int loop;
  float deltaTimeMass;
///////////////////////////////////////////////////////////////////////////////
  for (loop = 0; loop < m_ParticleCnt; loop++)
  {
    deltaTimeMass = deltaTime * initial->oneOverM;
    // DETERMINE THE NEW VELOCITY FOR THE PARTICLE
    target->v.x = initial->v.x + (source->f.x * deltaTimeMass);
    target->v.y = initial->v.y + (source->f.y * deltaTimeMass);
    target->v.z = initial->v.z + (source->f.z * deltaTimeMass);
    
    target->oneOverM = initial->oneOverM;
    
    // SET THE NEW POSITION
    target->pos.x = initial->pos.x + (deltaTime * source->v.x);
    target->pos.y = initial->pos.y + (deltaTime * source->v.y);
    target->pos.z = initial->pos.z + (deltaTime * source->v.z);
    
    initial++;
    source++;
    target++;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Function:	EulerIntegrate 
// Purpose:		Calculate new Positions and Velocities given a deltatime
// Arguments:	DeltaTime that has passed since last iteration
// Notes:		This integrator uses Euler's method
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::EulerIntegrate( float DeltaTime)
{
	// JUST TAKE A SINGLE STEP
	IntegrateSysOverTime(m_CurrentSys,m_CurrentSys, m_TargetSys,DeltaTime);
}

///////////////////////////////////////////////////////////////////////////////
// Function:	MidPointIntegrate 
// Purpose:		Calculate new Positions and Velocities given a deltatime
// Arguments:	DeltaTime that has passed since last iteration
// Notes:		This integrator uses the Midpoint method
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::MidPointIntegrate( float DeltaTime)
{
/// Local Variables ///////////////////////////////////////////////////////////
  float		halfDeltaT;
///////////////////////////////////////////////////////////////////////////////
  halfDeltaT = DeltaTime / 2.0f;
  
  // TAKE A HALF STEP AND UPDATE VELOCITY AND POSITION
  IntegrateSysOverTime(m_CurrentSys,m_CurrentSys,m_TempSys[0],halfDeltaT);
  
  // COMPUTE FORCES USING THESE NEW POSITIONS AND VELOCITIES
  ComputeForces(m_TempSys[0]);
  
  // TAKE THE FULL STEP WITH THIS NEW INFORMATION
  IntegrateSysOverTime(m_CurrentSys,m_TempSys[0],m_TargetSys,DeltaTime);
}

///////////////////////////////////////////////////////////////////////////////
// Function:	RK4Integrate 
// Purpose:		Calculate new Positions and Velocities given a deltatime
// Arguments:	DeltaTime that has passed since last iteration
// Notes:		This integrator uses the Runga-Kutta 4 method
//				This could use a generic function 4 times instead of unrolled
//				but it was easier for me to debug.  Fun for you to optimize.
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::RK4Integrate( float DeltaTime)
{
/// Local Variables ///////////////////////////////////////////////////////////
  int loop;
  float		halfDeltaT,sixthDeltaT;
  tParticle	*source,*target,*accum1,*accum2,*accum3,*accum4;
///////////////////////////////////////////////////////////////////////////////
  halfDeltaT = DeltaTime / 2.0f;		// SOME TIME VALUES I WILL NEED
  sixthDeltaT = 1.0f / 6.0f;
  
  // FIRST STEP
  source = m_CurrentSys;	// CURRENT STATE OF PARTICLE
  target = m_TempSys[0];	// TEMP STORAGE FOR NEW POSITION
  accum1 = m_TempSys[1];	// ACCUMULATE THE INTEGRATED VALUES
  for (loop = 0; loop < m_ParticleCnt; loop++)
  {
    accum1->f.x = halfDeltaT * source->f.x * source->oneOverM;
    accum1->f.y = halfDeltaT * source->f.y * source->oneOverM;
    accum1->f.z = halfDeltaT * source->f.z * source->oneOverM;
    
    accum1->v.x = halfDeltaT * source->v.x;
    accum1->v.y = halfDeltaT * source->v.y;
    accum1->v.z = halfDeltaT * source->v.z;
    // DETERMINE THE NEW VELOCITY FOR THE PARTICLE OVER 1/2 TIME
    target->v.x = source->v.x + (accum1->f.x);
    target->v.y = source->v.y + (accum1->f.y);
    target->v.z = source->v.z + (accum1->f.z);
    
    target->oneOverM = source->oneOverM;
    
    // SET THE NEW POSITION
    target->pos.x = source->pos.x + (accum1->v.x);
    target->pos.y = source->pos.y + (accum1->v.y);
    target->pos.z = source->pos.z + (accum1->v.z);
    
    source++;
    target++;
    accum1++;
  }
  
  ComputeForces(m_TempSys[0]);  // COMPUTE THE NEW FORCES
  
  // SECOND STEP
  source = m_CurrentSys;	// CURRENT STATE OF PARTICLE
  target = m_TempSys[0];	// TEMP STORAGE FOR NEW POSITION
  accum1 = m_TempSys[2];	// ACCUMULATE THE INTEGRATED VALUES
  for (loop = 0; loop < m_ParticleCnt; loop++)
  {
    accum1->f.x = halfDeltaT * target->f.x * source->oneOverM;
    accum1->f.y = halfDeltaT * target->f.y * source->oneOverM;
    accum1->f.z = halfDeltaT * target->f.z * source->oneOverM;
    accum1->v.x = halfDeltaT * target->v.x;
    accum1->v.y = halfDeltaT * target->v.y;
    accum1->v.z = halfDeltaT * target->v.z;
    
    // DETERMINE THE NEW VELOCITY FOR THE PARTICLE
    target->v.x = source->v.x + (accum1->f.x);
    target->v.y = source->v.y + (accum1->f.y);
    target->v.z = source->v.z + (accum1->f.z);
    
    target->oneOverM = source->oneOverM;
    
    // SET THE NEW POSITION
    target->pos.x = source->pos.x + (accum1->v.x);
    target->pos.y = source->pos.y + (accum1->v.y);
    target->pos.z = source->pos.z + (accum1->v.z);
    
    source++;
    target++;
    accum1++;
  }
  
  ComputeForces(m_TempSys[0]);  // COMPUTE THE NEW FORCES
  
  // THIRD STEP
  source = m_CurrentSys;	// CURRENT STATE OF PARTICLE
  target = m_TempSys[0];	// TEMP STORAGE FOR NEW POSITION
  accum1 = m_TempSys[3];	// ACCUMULATE THE INTEGRATED VALUES
  for (loop = 0; loop < m_ParticleCnt; loop++)
  {
    // NOTICE I USE THE FULL DELTATIME THIS STEP
    accum1->f.x = DeltaTime * target->f.x * source->oneOverM;
    accum1->f.y = DeltaTime * target->f.y * source->oneOverM;
    accum1->f.z = DeltaTime * target->f.z * source->oneOverM;
    accum1->v.x = DeltaTime * target->v.x;
    accum1->v.y = DeltaTime * target->v.y;
    accum1->v.z = DeltaTime * target->v.z;
    
    // DETERMINE THE NEW VELOCITY FOR THE PARTICLE
    target->v.x = source->v.x + (accum1->f.x);
    target->v.y = source->v.y + (accum1->f.y);
    target->v.z = source->v.z + (accum1->f.z);
    
    target->oneOverM = source->oneOverM;
    
    // SET THE NEW POSITION
    target->pos.x = source->pos.x + (accum1->v.x);
    target->pos.y = source->pos.y + (accum1->v.y);
    target->pos.z = source->pos.z + (accum1->v.z);
    
    source++;
    target++;
    accum1++;
  }
  
  ComputeForces(m_TempSys[0]);  // COMPUTE THE NEW FORCES
  
  // FOURTH STEP
  source = m_CurrentSys;	// CURRENT STATE OF PARTICLE
  target = m_TempSys[0];	// TEMP STORAGE FOR NEW POSITION
  accum1 = m_TempSys[4];	// ACCUMULATE THE INTEGRATED VALUES
  for (loop = 0; loop < m_ParticleCnt; loop++)
  {
    // NOTICE I USE THE FULL DELTATIME THIS STEP
    accum1->f.x = DeltaTime * target->f.x * source->oneOverM;
    accum1->f.y = DeltaTime * target->f.y * source->oneOverM;
    accum1->f.z = DeltaTime * target->f.z * source->oneOverM;
    
    accum1->v.x = DeltaTime * target->v.x;
    accum1->v.y = DeltaTime * target->v.y;
    accum1->v.z = DeltaTime * target->v.z;
    
    // THIS TIME I DON'T NEED TO COMPUTE THE TEMPORARY POSITIONS
    source++;
    target++;
    accum1++;
  }
  
  source = m_CurrentSys;	// CURRENT STATE OF PARTICLE
  target = m_TargetSys;
  accum1 = m_TempSys[1];
  accum2 = m_TempSys[2];
  accum3 = m_TempSys[3];
  accum4 = m_TempSys[4];
  for (loop = 0; loop < m_ParticleCnt; loop++)
  {
    // DETERMINE THE NEW VELOCITY FOR THE PARTICLE USING RK4 FORMULA
    target->v.x = source->v.x + ((accum1->f.x + ((accum2->f.x + accum3->f.x) * 2.0f) + accum4->f.x) * sixthDeltaT);
    target->v.y = source->v.y + ((accum1->f.y + ((accum2->f.y + accum3->f.y) * 2.0f) + accum4->f.y) * sixthDeltaT);
    target->v.z = source->v.z + ((accum1->f.z + ((accum2->f.z + accum3->f.z) * 2.0f) + accum4->f.z) * sixthDeltaT);
    // DETERMINE THE NEW POSITION FOR THE PARTICLE USING RK4 FORMULA
    target->pos.x = source->pos.x + ((accum1->v.x + ((accum2->v.x + accum3->v.x) * 2.0f) + accum4->v.x) * sixthDeltaT);
    target->pos.y = source->pos.y + ((accum1->v.y + ((accum2->v.y + accum3->v.y) * 2.0f) + accum4->v.y) * sixthDeltaT);
    target->pos.z = source->pos.z + ((accum1->v.z + ((accum2->v.z + accum3->v.z) * 2.0f) + accum4->v.z) * sixthDeltaT);
    
    source++;
    target++;
    accum1++;
    accum2++;
    accum3++;
    accum4++;
  }
  
}

// CHECKS FOR COLLISIONS BETWEEN EACH PARTICLE WITH THE COLLISION WALLS AND EVERY CYLINDER
// RETURNS THE COLLISION STATE OF THE SYSTEM (NOT_COLLIDING, PENETRATING OR COLLIDING)
// IF COLLIDING, UPDATES THE LIST OF PARTICLES UNDERGOING COLLISION.
int CPhysEnv::CheckForCollisions( tParticle	*system )
{
  int collisionState = NOT_COLLIDING;
  float const depthEpsilon = 0.01f;
  
  int loop;
  tParticle *curParticle;
  
  m_ContactCnt = 0;		// THERE ARE CURRENTLY NO CONTACTS
  m_PenetrateCnt = 0;
  
  curParticle = system;
  for (loop = 0; (loop < m_ParticleCnt); loop++,curParticle++)
  {    
    // CHECK THE MAIN BOUNDARY PLANES FIRST
    //
    for(int planeIndex = 0;(planeIndex < m_CollisionPlaneCnt);planeIndex++)
    {
      tCollisionPlane *plane = &m_CollisionPlane[planeIndex];
      
      float axbyczd = DotProduct(&curParticle->pos,&plane->normal) + plane->d;
      // Handle ground-cloth interaction. Note that we don't explicitly resolve interpenetration
      // We assume that the cloth is above the ground to begin with.
      if(axbyczd < depthEpsilon) 
      {
        float relativeVelocity = DotProduct(&plane->normal,&curParticle->v);
        
        if(relativeVelocity < 0.0f)
        {
          collisionState = COLLIDING;
          m_Contact[m_ContactCnt].particle = loop; 
          memcpy(&m_Contact[m_ContactCnt].normal,&plane->normal,sizeof(tVector));
          m_ContactCnt++;
        }
	  }      

      if(axbyczd < 0.0f ) //-depthEpsilon) 
	  {
		  collisionState = PENETRATING;
	      m_Penetrate[m_PenetrateCnt].particle = loop; 
          memcpy(&m_Penetrate[m_PenetrateCnt].normal,&plane->normal,sizeof(tVector));
		  m_Penetrate[m_PenetrateCnt].dist = axbyczd; // distance to the penetration surface
          m_PenetrateCnt++;
	  }
    }
    
    // Handle Collision with Cylinders. 
    // YOU HAVE TO handle both collisions and penetrations explicitly
    if (m_CollisionActive)
    {

       //15497: WRITE CODE HERE

	   // debug: a cylinder on the ground
	   /*tCollisionCyl myCyl;
	   MAKEVECTOR(myCyl.axis, 0.0, 0.0, 1.0);
	   myCyl.len = 2.0;
	   MAKEVECTOR(myCyl.pos, 0.0, 0.0, 0.0);
	   myCyl.radius = 1.2;
	   for(int cylinderIndex = 0; cylinderIndex < 1; cylinderIndex++)*/

	   for(int cylinderIndex = 0; cylinderIndex < m_CylCnt; cylinderIndex++)
	   {

		  //tCollisionCyl *cylinder = &myCyl;
		  tCollisionCyl *cylinder = &m_Cyl[cylinderIndex];
		  tCollisionCylInfo curCylInfo = DistPointCylinder(&curParticle->pos, cylinder);
		
	      if( VectorLength(&curCylInfo.tanVect) < (cylinder->len/2.0) )
		  {
		     float dist2axis = VectorLength(&curCylInfo.normVect);

		     if(dist2axis < cylinder->radius)
			 {
			    collisionState = PENETRATING;
				//printf("penetrate point: %f, %f, %f, cyl:%d\n", cylinder->pos.x, cylinder->pos.y, cylinder->pos.z, cylinderIndex); 
				// note that a particle may pentrate multiple objects/planes at the same time in rare cases
				// this is automatically handled by including an partile into the penetration array
				// for each penetration case; that is, the particle may appear in m_Pentrate[] for mutiple times.
				// When doing ResolvePenetration(), these forces are summed up.
				m_Penetrate[m_PenetrateCnt].particle = loop; 
		
				// curCylInfo->normVect is a distance vector, we need to normalize it
				NormalizeVector(&curCylInfo.normVect);
				MAKEVECTOR( m_Penetrate[m_PenetrateCnt].normal, 
							curCylInfo.normVect.x,
							curCylInfo.normVect.y,
							curCylInfo.normVect.z);

				m_Penetrate[m_PenetrateCnt].dist = dist2axis - cylinder->radius;  
				m_PenetrateCnt++;
			 }
			 else if(dist2axis < (cylinder->radius+depthEpsilon) )
			 {
				NormalizeVector(&curCylInfo.normVect);
				float relativeVelocity = DotProduct(&curCylInfo.normVect, &curParticle->v);
				if(relativeVelocity < 0.0f)
				{
					collisionState = COLLIDING;
					m_Contact[m_ContactCnt].particle = loop; 
					memcpy(&m_Contact[m_ContactCnt].normal,&curCylInfo.normVect,sizeof(tVector));
					m_ContactCnt++;
				}
			 }

		  }		  
		
	   }
	}

  }
  return collisionState;
}

// RESOLVING COLLISIONS USING IMPULSE METHOD
// Change the velocity of the particle which has not penetrated but is sufficiently close
// Apply impulse if dist >0 and dist < user_defined_epsilon

// Vn = dot(N,Vold)*N
// Vt = Vold - Vn
// Vnew = Vt - Kr*Vn
void CPhysEnv::ResolveCollisions(tParticle *system)
{
	tContact *contact;
	tParticle *particle;
	tVector Vn, Vt;

	contact = m_Contact;
	for(int i=0; i < m_ContactCnt; i++, contact++)
	{
		particle = &system[contact->particle];
		// Vn = dot(N,Vold)*N
		ScaleVector(&contact->normal, DotProduct(&contact->normal, &particle->v), &Vn);
		// Vt = Vold - Vn
		VectorDifference(&particle->v, &Vn, &Vt);
		// Vnew = Vt - Kr*Vn
		ScaleVector(&Vn, m_Kr, &Vn); 
		VectorDifference(&Vt, &Vn, &particle->v);
	}
}

// RESOLVING PENETRATIONS USING PENALTY FUNCTIONS
// Apply a spring force for points that have already penetrated the surface.
// The force should be directed outwards from the surface of the cylinder.
void CPhysEnv::ResolvePenetrations(tParticle *system)
{
	//15497: WRITE CODE HERE

	tPenetrate *penetrate;
	tParticle *particle;
	tVector springForce;
	float Hterm, Dterm;
	const float Khpen = m_PenaltyKs*10, Kdpen = Khpen/100.0;

	penetrate = m_Penetrate;
	for(int i=0; i < m_PenetrateCnt; i++, penetrate++)
	{
		particle = &system[penetrate->particle];
		Hterm = -Khpen*penetrate->dist;
		Dterm = -Kdpen*DotProduct(&particle->v, &penetrate->normal);
		ScaleVector(&penetrate->normal, Hterm+Dterm, &springForce);
		VectorSum(&particle->f, &springForce, &particle->f);
	}
}

// CLEAR THE LISTS STORING COLLISION AND PENETRATION INFORMATION
void CPhysEnv::ClearCollisionBuffers()
{
  if(m_Contact)
    free(m_Contact);
  if(m_Penetrate)
    free(m_Penetrate);
  
  m_Contact = (tContact *)malloc(sizeof(tContact) * m_ParticleCnt * 5);
  m_ContactCnt = 0;
  
  m_Penetrate = (tPenetrate *)malloc(sizeof(tPenetrate) * m_ParticleCnt * 5);
  m_PenetrateCnt = 0;
}

// The main simulation loop. At each time step, compute the net forces, resolve collisions and integrate

void CPhysEnv::Simulate(float DeltaTime, bool running)
{
 //15497: WRITE CODE HERE
    tParticle *tmpSys; 
	int collisionState = NOT_COLLIDING;

	if(running)
	{

		collisionState = CheckForCollisions(m_CurrentSys);

		if(  m_ContactCnt != 0)
			ResolveCollisions(m_CurrentSys);

		// note: force would be clear in ComputeForce()
		// this should be called before penalty forces are added
		ComputeForces(m_CurrentSys);

		// PENETRATING
		if(m_PenetrateCnt != 0)		
			ResolvePenetrations(m_CurrentSys);

		switch (m_IntegratorType)
		{
			case RK4_INTEGRATOR:	
				RK4Integrate(DeltaTime);
			break;
			case MIDPOINT_INTEGRATOR:
				MidPointIntegrate(DeltaTime);
			break;
			case EULER_INTEGRATOR:
				EulerIntegrate(DeltaTime);
			break;
		}

	
#ifdef WRT_DEBUG
		fprintParticlePos(0,0);
		fprintf(fdebug.fp, " suspend \tpos = (%f, %f, %f)\n", m_SuspensionPoints[0].x, 
			m_SuspensionPoints[0].y, m_SuspensionPoints[0].z);
		fprintParticleFrc(0,0);
		fprintParticlePos(0,14);		
		fprintf(fdebug.fp, " suspend \tpos = (%f, %f, %f)\n", m_SuspensionPoints[1].x, 
			m_SuspensionPoints[1].y, m_SuspensionPoints[1].z);
		fprintParticleFrc(0,14);
		fdebug.flush();
#endif
		tmpSys = m_CurrentSys;
		m_CurrentSys = m_TargetSys;
		m_TargetSys = tmpSys;

		//memcpy(m_CurrentSys,m_TargetSys,sizeof(tParticle) * m_ParticleCnt);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Function:	AddCollisionSphere 
// Purpose:     Add a collision sphere to the system
///////////////////////////////////////////////////////////////////////////////
// NOT USED for this assignment, ignore
void CPhysEnv::AddCollisionSphere(tVector pos, float radius)
{
/// Local Variables ///////////////////////////////////////////////////////////
  tCollisionSphere *temparray;
  temparray = (tCollisionSphere *)malloc(sizeof(tCollisionSphere) * (m_SphereCnt+1)); 	
  if (m_SphereCnt > 0)
  {
    memcpy(temparray,m_Sphere,sizeof(tCollisionSphere) * m_SphereCnt);
    free(m_Sphere);
  }
  
  //MAKEVECTOR(temparray[m_SphereCnt].pos,dialog.m_XPos,dialog.m_YPos, dialog.m_ZPos)
  //temparray[m_SphereCnt].radius = dialog.m_Radius;
  temparray->pos = pos;
  temparray->radius = radius;
  
  m_Sphere = temparray;
  m_SphereCnt++;
}

// Add a Collision Cylinder to the system
void CPhysEnv::AddCollisionCyl(tVector com, float len, float radius, tVector axis)
{ 
  m_Cyl[m_CylCnt].pos = com;
  m_Cyl[m_CylCnt].len = len;
  m_Cyl[m_CylCnt].radius = radius;
  NormalizeVector(&axis);
  m_Cyl[m_CylCnt].axis = axis;
  m_CylCnt++;
}

// Clear the list of Collision Cylinders 
void CPhysEnv::ClearCollisionCyl()
{
  if(m_Cyl)
  {
    m_CylCnt = 0;
    free(m_Cyl);
  }
  m_Cyl = (tCollisionCyl*)malloc(sizeof(tCollisionCyl) * (NUM_BONES_IN_ASF_FILE));
  m_CylCnt = 0;
}

// Compute the distance between a Cylinder and a Point. 
// The distance info is stored in the tCollisionCylInfo structure
tCollisionCylInfo CPhysEnv::DistPointCylinder(tVector *pointPos, tCollisionCyl *cylinder)
{
 //15497: WRITE CODE HERE

	tVector R, Rt, Rn; 
	tCollisionCylInfo collisionCylInfo;

	// vector from cylinder's center to the point, R = p - c
    VectorDifference(pointPos, &cylinder->pos, &R); 	
	// get tangent term by projection, Rt = (R*a)a, note that a has unit length
	ScaleVector(&cylinder->axis, DotProduct(&R, &cylinder->axis), &Rt);
	// normal term, Rn = R - Rt
	VectorDifference(&R, &Rt, &Rn);

	MAKEVECTOR(collisionCylInfo.normVect, Rn.x, Rn.y, Rn.z);
    MAKEVECTOR(collisionCylInfo.tanVect, Rt.x, Rt.y, Rt.z);
	return collisionCylInfo;
}
		
// debug functions
void CPhysEnv::fprintParticlePos(int v, int u)
{
     fprintf(fdebug.fp, "particle(%d,%d)\tpos = (%f, %f, %f)\n", u, v, 
		 m_CurrentSys[v*m_u + u].pos.x, 
		 m_CurrentSys[v*m_u + u].pos.y, 
		 m_CurrentSys[v*m_u + u].pos.z);
}

void CPhysEnv::fprintParticleFrc(int v, int u)
{
     fprintf(fdebug.fp, "particle(%d,%d)\tforce = (%f, %f, %f)\n", u, v, 
		 m_CurrentSys[v*m_u + u].f.x, 
		 m_CurrentSys[v*m_u + u].f.y, 
		 m_CurrentSys[v*m_u + u].f.z);
}