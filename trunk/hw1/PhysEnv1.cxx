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
#include "PhysEnv1.h"
#include "MathDefs.h"

#include "debugfile.h"
extern DebugFile fdebug;

/////////////////////////////////////////////////////////////////////////////
// CPhysEnv

// INITIALIZE THE SIMULATION WORLD
CPhysEnv::CPhysEnv()
{
	// select the integrator
	//m_IntegratorType = EULER_INTEGRATOR;
	//m_IntegratorType = MIDPOINT_INTEGRATOR;
	m_IntegratorType = RK4_INTEGRATOR;
	//m_IntegratorType = VERLET_INTEGRATOR;
	m_u = 9;
	m_v = 9;
	//m_AttachPts[0] = -1;
	//m_AttachPts[1] = -1;
	m_AttachPts = NULL;
	m_SuspensionPoints = NULL;
	m_AttachPtsCnt = 0;
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
	m_AttachForceKs = 10000.0f; //ATTACH (cloth/body) FORCE CONSTANT
	m_AttachForceKd = 1.5f;
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

		glDisable(GL_CULL_FACE);
		if(loop>0)
		{
			for (int i=0; i<m_u-1; i++)
			{
				for(int j=0; j<m_v-1; j++)
				{
					glBegin(GL_QUADS);
					glVertex3fv((float *)&temp_array[j*m_u+i]);
					glVertex3fv((float *)&temp_array[j*m_u+i+1]);
					glVertex3fv((float *)&temp_array[(j+1)*m_u+i+1]);
					glVertex3fv((float *)&temp_array[(j+1)*m_u+i]);
					glEnd();
				}
			}
		}
		glEnable(GL_CULL_FACE);
		free(temp_array);
	}

	//glDisable(GL_DEPTH_TEST);
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
			
			glEnd();
			if (m_AttachedtoBody)	// DRAW MOUSESPRING FORCE
			{
				for(int i=0; i<m_AttachPtsCnt; i++)
				{
					glColor3f(0.8f,0.0f,0.8f);
					glVertex3fv((float *)&m_CurrentSys[m_AttachPts[i]].pos);
					glVertex3fv((float *)&m_SuspensionPoints[i]);
				}
			}
			glEnd();
		}
		//glEnable(GL_DEPTH_TEST);
	}
	/*    
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
	*/
}


void CPhysEnv::SetWorldParticles(tTexturedVertex *coords,int particleCnt, bool isAttached, int *attachPts, int attachPtsCnt)
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
	for (int i = 0; i < 5; i++)
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

	tVector p1, p2;
	float shift_x, shift_y, shift_z;

	// Calculate the shift amount from m_CurrentSys to m_SuspensionPoints (mid point to mid point)
	VectorSum(&(m_CurrentSys[m_AttachPts[0]].pos), &(m_CurrentSys[m_AttachPts[1]].pos), &p1);
	ScaleVector(&p1, 0.5f, &p1);
	VectorSum(&m_SuspensionPoints[0], &m_SuspensionPoints[1], &p2);
	ScaleVector(&p2, 0.5f, &p2);
	shift_x = p2.x - p1.x;
	shift_y = p2.y - p1.y;
	shift_z = p2.z - p1.z;
	// Translate the cloth to the position behind the suspension point
	for(int i = 0; i < m_ParticleCnt; i++)
	{
		m_CurrentSys[i].pos.x += shift_x;
		m_CurrentSys[i].pos.y += shift_y;
		m_CurrentSys[i].pos.z += shift_z;
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
		m_AttachPtsCnt = attachPtsCnt;
	}
}


///////////////////////////////////////////////////////////////////////////////
// Function:	FreeSystem
// Purpose:		Remove all particles and clear it out
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::FreeSystem()
{
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

// RESET THE SIM TO INITIAL VALUES
void CPhysEnv::ResetWorld()
{
	memcpy(m_CurrentSys,m_ParticleSys[2],sizeof(tParticle) * m_ParticleCnt);
	memcpy(m_TargetSys,m_ParticleSys[2],sizeof(tParticle) * m_ParticleCnt);

    // set initialization condition of particle position
	if(m_AttachedtoBody && m_AttachPtsCnt == 2)
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
/*
//	Reset the clothe to a small tie?
		dx = -0.8 / (float)(m_v - 1); 
		//dx = 0.8 / (float)(m_v - 1); 
		for(int i=1; i<m_v; i++)	
			for(int j=0; j<m_u; j++)
			{
				m_CurrentSys[i*m_u+j].pos.x = m_CurrentSys[(i-1)*m_u+j].pos.x + dx;
				m_CurrentSys[i*m_u+j].pos.y = m_CurrentSys[(i-1)*m_u+j].pos.y;
				m_CurrentSys[i*m_u+j].pos.z = m_CurrentSys[(i-1)*m_u+j].pos.z;
			}*/

	}
}

void CPhysEnv::SetWorldProperties()
{
// You can set the world parameters (spring constants, mass, dampers, etc) from the GUI
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
		spring = &m_Spring[m_SpringCnt++];//指到最後一個
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
	tSpring		*s;
	tParticle	*p1, *p2;
	tVector		posDiff, vDiff;	
	double		posDistance;
	tVector		fs, fd;

	// add your code here
	int i;
	//Add gravity to all points
	for (i = 0; i < m_ParticleCnt; i++)
		MAKEVECTOR(m_CurrentSys[i].f, m_Gravity.x, m_Gravity.y, m_Gravity.z);
 
	//Add spring-damper force
	for (int i = 0; i < m_SpringCnt; i++)
	{
		s = &m_Spring[i];
		p1 = &system[m_Spring[i].p1];
		p2 = &system[m_Spring[i].p2];
		VectorDifference(&p1->pos, &p2->pos, &posDiff);
		VectorDifference(&p1->v, &p2->v, &vDiff);
		posDistance = sqrt( VectorLength(&posDiff) );
		// Calculate spring force
		ScaleVector(&posDiff, (-1) * (s->Ks) * (posDistance - s->restLen) / posDistance, &fs);

		// Calculate damper force
		ScaleVector(&posDiff, (-1) * DotProduct(&vDiff, &posDiff) / (posDistance * posDistance), &fd);

		// Add both to force accumulator
		VectorSum(&p1->f, &fs, &p1->f);
		VectorSum(&p1->f, &fd, &p1->f);
		VectorDifference(&p2->f, &fs, &p2->f);
		VectorDifference(&p2->f, &fd, &p2->f);
	}

	//Constrain attached points in the cloth to be attached to the body by springs

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

void CPhysEnv::VerletIntegrate ( float DeltaTime)
{
	// add your code here
}

///////////////////////////////////////////////////////////////////////////////
// Function:	MidPointIntegrate 
// Purpose:		Calculate new Positions and Velocities given a deltatime
// Arguments:	DeltaTime that has passed since last iteration
// Notes:		This integrator uses the Midpoint method
///////////////////////////////////////////////////////////////////////////////
void CPhysEnv::MidPointIntegrate( float DeltaTime)
{
	//add your code here
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
	//add your code here
}

// CHECKS FOR COLLISIONS BETWEEN EACH PARTICLE WITH THE COLLISION WALLS AND EVERY CYLINDER
// RETURNS THE COLLISION STATE OF THE SYSTEM (NOT_COLLIDING, PENETRATING OR COLLIDING)
// IF COLLIDING, UPDATES THE LIST OF PARTICLES UNDERGOING COLLISION.
int CPhysEnv::CheckForCollisions( tParticle	*system )
{
	//add your code here
	return 1;
}

// RESOLVING COLLISIONS USING IMPULSE METHOD
// Change the velocity of the particle which has not penetrated but is sufficiently close
// Apply impulse if dist >0 and dist < user_defined_epsilon

// Vn = dot(N,Vold)*N
// Vt = Vold - Vn
// Vnew = Vt - Kr*Vn
void CPhysEnv::ResolveCollisions(tParticle *system)
{
	//add your code here
}

// RESOLVING PENETRATIONS USING PENALTY FUNCTIONS
// Apply a spring force for points that have already penetrated the surface.
// The force should be directed outwards from the surface of the cylinder.
void CPhysEnv::ResolvePenetrations(tParticle *system)
{
	//add your code here
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
			case VERLET_INTEGRATOR:
				VerletIntegrate(DeltaTime);
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

		ClearCollisionBuffers();

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
