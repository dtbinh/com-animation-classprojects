#ifndef PARTICLE_H
#define PARTICLE_H

//~~~~~~ Particle class. Strand class. Force Gravity, AirVis..., Wind class. ~~~~~~~~~~~~~~~~~~~
#include	<cassert>
#include	<cmath>
#include	<vector>
#include	<Ogre.h>
using namespace Ogre;
#include	"mat_face.h"	// Face data structure
#include	"aabb.h"

//typedef double DP;	//double precision floating point �W�پ� C++�ƭȤ�k����
typedef float DP;
class AppParticle;
class Spring;
class Strand;

enum Status { FREE, COLLIDE, CONTACT, PENENTRATE };
class AppParticle
{
public:
	Vector3 x;
	Vector3 v;
	Vector3 a;
	DP invM;		// inverse Mass �Y�g
	// @todo �[ collision state keeping
	Status status;
	Face *hitTri;
public:
	AppParticle();
	AppParticle(const AppParticle & p);
	
	/* Debug info */
	/*
	inline void printInfo()
	{
		printf("------------particle data------------\n");
		printf("x:");
		x.printInfo();
		printf("v:");
		v.printInfo();
		printf("a:");
		a.printInfo();
		// @todo cState->printInfo
		printf("\n---------------------------------\n");
	}
	*/
};

/****************************************************************************/
/*   Strand: point-Mass Spring Line                                         */
/****************************************************************************/
// �@�� strand line �W�� particle �Ҧ��ڪ���n => �y���b�|
//#define P_BALL_R 0.2f		// particle's ball's radius
//#define P_BALL_R2 0.04f		//�b�|����
#define P_BALL_R 0.03f		// particle's ball's radius
#define P_BALL_R2 0.0009f		//�b�|����

class Strand{
public:
	DP pieceLen;
	std::vector< AppParticle > pList;	// particle list
	std::vector< Spring > sList;	// spring list
	BoundBox m_box;					// bounding box
	int pTotal, sTotal;
	
	void buildBox();				// called after pList is setup
	void updateBox();
	//~~~~ Constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Strand()
		:pieceLen(0), pTotal(0), sTotal(0)
	{}
	Strand( const Strand & s )
		:pieceLen( s.pieceLen ), pList( s.pList ), pTotal( s.pTotal ), 
		sList( s.sList ), sTotal( s.sTotal ), m_box( s.m_box )
	{}
	//subscript operator for non-const objects returns modifiable lvalue
	AppParticle& operator[](int index)
	{
		return pList[index];	//reference
	}
	AppParticle operator[]( int index ) const
	{
		return pList[index];	//copy of this element
	}
};

/****************************************************************************/
/*   Spring                                                                 */
/****************************************************************************/
//�̤j�i���i�����פ��
#define MAX_STRETCH_RATIO 0.1f

class Spring{
private:
	DP len0;	//�۵M����
	DP k;		//k spring-constant
	DP kd;		// damping coefficient
	//@Todo state is over stretched ??
	int pIndex[2];	//particles' index connected by this spring
					//�O���o�Ӽu®�s�����I�b particle array ����index
public:
	Spring();
	Spring( const Spring &spr );

	void setLen0( DP len0 )
	{
		this->len0 = len0;
	}
	void setK( DP k )
	{
		this->k = k;
	}
	void setKd( DP kd )
	{
		this->kd = kd;
	}

	void setParticleIndex( int i, int j )
	{
		pIndex[0] = i;
		pIndex[1] = j;
	}
	void getParticleIndex( int *i, int *j )
	{
		*i = pIndex[0];
		*j = pIndex[1];
	}

	/* method */
	//bool isOverStretched();
	//void applyAccel2particle();
	
	bool isOverStretched( const std::vector< AppParticle > &particleArray );
	void applyAccel2particle( std::vector< AppParticle> &particleArray );

	/* Debug info */
	void printInfo()
	{
		printf("------------particle data------------\n");
		printf("(len0, k, kd):(%.2f,%.2f,%.2f)", len0, k, kd);
		// @todo cState->printInfo
		printf("\n---------------------------------\n");
	}
};
/****************************************************************************/
// ���O
/****************************************************************************/
class Gravity{
private:
	Vector3 G;
public:
	Gravity()
		:G( 0.0, -9.8f, 0.0 )
	{
	}
	Vector3 getG() const
	{
		return G;
	}
	void setG( const Vector3 &g )
	{
		G = g;
	}
	//�[�W�@�ΤO���[�t��
	void applyAccel( AppParticle *pPtr )
	{
		if( pPtr->invM != 0 ){
			pPtr->a = G + pPtr->a;
		}
	}
};
/****************************************************************************/
//  �Ů��H���O
/****************************************************************************/
class AirViscousity{
private:
	DP k;	//�H���Y��
public:
	AirViscousity()
		:k( 0.1f )
	{}
	void setCoefficient( DP k )
	{
		assert( k >= 0 );
		this->k = k;
	}
	DP getCoefficient() const
	{
		return k;
	}
	void applyAccel( AppParticle *pPtr )
	{
		pPtr->a = pPtr->a + ( -k * pPtr->invM * pPtr->v );
	}
};

// Wind
class Wind{
private:
	Vector3 windF;
public:
	Wind()
	{
		windF = Vector3( 0, 0, 0.45 );
	}

	void setWind( const Vector3 &F )
	{
		windF = F;
	}

	void applyAccel( AppParticle *pPtr, float time )
	{
		//pPtr->a = pPtr->a + ( pPtr->invM * abs( sin( time )) * windF );
		pPtr->a = pPtr->a + ( pPtr->invM * sin( time ) * windF );

	}
};

#endif