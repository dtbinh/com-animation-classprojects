#ifndef _PHYSENV_H
#define _PHYSENV_H

// PhysEnv.h : header file
//
#include "MathDefs.h"
#include <stdio.h>
#define EPSILON  0.000001f				// ERROR TERM
#define DEFAULT_DAMPING		0.002f

enum tCollisionTypes
{
	NOT_COLLIDING,
	PENETRATING,
	COLLIDING
};

enum tIntegratorTypes
{
	EULER_INTEGRATOR,
	MIDPOINT_INTEGRATOR,
	RK4_INTEGRATOR
};


// CLASSIFY THE SPRINGS SO I CAN HANDLE THEM SEPARATELY
enum tSpringTypes
{
	MANUAL_SPRING,
	STRUCTURAL_SPRING,
	SHEAR_SPRING,
	BEND_SPRING
};

// TYPE FOR A PLANE THAT THE SYSTEM WILL COLLIDE WITH
struct tCollisionPlane
{
	tVector normal;			// inward pointing
        float	d;				// ax + by + cz + d = 0
};

// TYPE FOR A PHYSICAL PARTICLE IN THE SYSTEM
struct tParticle
{
	tVector pos;		// Position of Particle
        tVector v;			// Velocity of Particle
	tVector f;			// Force Acting on Particle
	float	oneOverM;	// 1 / Mass of Particle
};

// TYPE FOR CONTACTS THAT ARE FOUND DURING SIM
struct tContact
{
	int		particle;	// Particle Index
        tVector normal;		// Normal of Collision plane
};

// TYPE FOR CONTACTS THAT ARE FOUND DURING SIM
struct tPenetrate
{
	int		particle;	// Particle Index
        tVector normal;		// Normal of Closest point on penetration surface
	float	dist;		// Penetration depth
};


// TYPE FOR COLLISION SPHERES IN SYSTEM
struct tCollisionSphere
{
	float	radius;		// RADIUS OF SPHERE
        tVector pos;		// POSITION OF SPHERE
};

// TYPE FOR SPRINGS IN SYSTEM
struct tSpring
{
	int		p1,p2;		// PARTICLE INDEX FOR ENDS
	float	restLen;	// LENGTH OF SPRING AT REST
	float	Ks;			// SPRING CONSTANT
	float	Kd;			// SPRING DAMPING
	int		type;		// SPRING TYPE - USED FOR SPECIAL FEATURES
};

// TYPE FOR CYLINDRICAL COLLISION OBJECTS OF BODY
struct tCollisionCyl
{
	tVector  pos;		// Position of the center of the cylinder (COM)
	float    len;		// Length of the cylinder
	float    radius;	// Radius of the cylinder
	tVector  axis;		// Cylinder axis
};

//TYPE TO STORE THE COLLISION INFO BETWEEN A POINT AND A CYLINDER
struct tCollisionCylInfo
{
	//bool	boundary;	// True if the particle projects onto the cylinder
	tVector	normVect;	// Normal from the point to the axis, directed outwards from the cylinder
						// This vector is not normalized, it encodes the distance from point to cylinder
	tVector tanVect; 
};

class CPhysEnv
{
// Construction
public:
  CPhysEnv();
  void SetClothDimensions(int u, int v);
  void RenderWorld();
  void SetWorldParticles(tTexturedVertex *coords,int particleCnt, bool isAttached, int pt1, int pt2);
  void ResetWorld();
  void Simulate(float DeltaTime,bool running);
  void ApplyUserForce(tVector *force);
  void SetMouseForce(int deltaX,int deltaY, tVector *localX, tVector *localY);
  
  void AddSpring();
  void AddSpring(int v1, int v2,float Ksh,float Ksd, int type);
  void SetWorldProperties();
  void FreeSystem();
  void LoadData(FILE *fp);
  void SaveData(FILE *fp);
  void AddCollisionSphere(tVector com, float radius);
  void AddCollisionCyl(tVector com, float len, float radius, tVector axis);
  void ClearCollisionCyl();
  
  
  bool				m_UseGravity;			// SHOULD GRAVITY BE ADDED IN
  bool				m_UseDamping;			// SHOULD DAMPING BE ON
  bool				m_UserForceActive;		// WHEN USER FORCE IS APPLIED
  bool				m_DrawSprings;			// DRAW THE SPRING LINES
  bool				m_DrawVertices;			// DRAW VERTICES
  bool				m_MouseForceActive;		// MOUSE DRAG FORCE
  bool				m_CollisionActive;		// COLLISION SPHERES ACTIVE
  bool				m_CollisionRootFinding;	// AM I SEARCHING FOR A COLLISION
  bool				m_DrawStructural;		// DRAW STRUCTURAL CLOTH SPRINGS
  bool				m_DrawShear;			// DRAW SHEAR CLOTH SPRINGS
  bool				m_DrawBend;				// DRAW BEND CLOTH SPRINGS
  bool				m_AttachedtoBody;		// IS THE CLOTH ATTACHED TO BODY
  int					m_IntegratorType;
  int					m_SphereCnt;
  int					m_CylCnt;
  tVector				m_SuspensionPoints[2];	// SUSPENSION POINTS
  tCollisionSphere	*m_Sphere;
  tCollisionCyl		*m_Cyl;
  
  
// Attributes
private:
  int					m_u,m_v;				// DIMENSIONS OF THE CLOTH PATCH
  float				m_WorldSizeX,m_WorldSizeY,m_WorldSizeZ;
  tVector				m_Gravity;				// GRAVITY FORCE VECTOR
  tVector				m_UserForce;			// USER FORCE VECTOR
  float				m_UserForceMag;			// MAGNITUDE OF USER FORCE
  float				m_Kd;					// DAMPING FACTOR
  float				m_Kr;					// COEFFICIENT OF RESTITUTION
  float				m_Ksh;					// HOOK'S SPRING CONSTANT (used for user dragging force)
  float				m_Ksd;					// SPRING DAMPING
  float				m_MouseForceKs;			// MOUSE SPRING COEFFICIENT
  float				m_AttachForceKs;		// ATTACH (cloth/body) SPRING COEFFICIENT
  float				m_AttachForceKd;		// ATTACH DAMPING COEFFICIENT
  float				m_PenaltyKs;			// PENALTY FUNCTION SPRING COEFFICIENT
  tCollisionPlane		*m_CollisionPlane;		// LIST OF COLLISION PLANES
  int					m_CollisionPlaneCnt;			
  tContact			*m_Contact;				// LIST OF POSSIBLE COLLISIONS
  int					m_PenetrateCnt;			// PENETRATION COUNT
  tPenetrate			*m_Penetrate;			// LIST OF PENETRATED POINTS
  int					m_ContactCnt;			// COLLISION COUNT
  tParticle			*m_ParticleSys[3];		// LIST OF PHYSICAL PARTICLES
  tParticle			*m_CurrentSys,*m_TargetSys;
  tParticle			*m_TempSys[5];			// SETUP FOR TEMP PARTICLES USED WHILE INTEGRATING
  int					m_ParticleCnt;
  tSpring				*m_Spring;				// VALID SPRINGS IN SYSTEM
  int					m_SpringCnt;		
  int					m_Pick[2];				// INDEX COUNTERS FOR SELECTING
  tVector				m_MouseDragPos[2];		// POSITION OF DRAGGED MOUSE VECTOR
  int					m_AttachPts[2]; // INDEX THE CLOTH POINTS ATTACHED TO BODY
  
// Operations
private:
  inline void	IntegrateSysOverTime(tParticle *initial,tParticle *source, tParticle *target, float deltaTime);
  void	RK4Integrate( float DeltaTime);
  void	MidPointIntegrate( float DeltaTime);
  void	EulerIntegrate( float DeltaTime);
  void	ComputeForces( tParticle	*system );
  int	CheckForCollisions( tParticle	*system );
  void	ResolveCollisions(tParticle	*system);
  void	ResolvePenetrations(tParticle	*system);
  void	ClearCollisionBuffers();
  tCollisionCylInfo	DistPointCylinder(tVector *pointPos, tCollisionCyl *cylinder);

//Debug
  void fprintParticlePos(int u, int v);
  void fprintParticleFrc(int u, int v);

// Implementation
public:
  virtual ~CPhysEnv();
  
};

/////////////////////////////////////////////////////////////////////////////

#endif // _PHYSENV_H


