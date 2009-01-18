#ifndef _HAIR_H_
#define _HAIR_H_

#include "AppBox.h"
#include <Ogre.h>

using namespace Ogre;

/*
	Hair class
*/
class Hair
{
protected:
	Vector3 mRootPos;						//	Root position
	bool	mInterpolated;					//	Indicate this hair is interpolated from its neighbors
	int		mNeighbors[3];					//	Neighbor hairs(Three at most)
	std::vector<Vector3> mParticlePoses;	// Positions of particles
	AppBox* mHairEdges;						// Visual components of hair edges
	
	
public:
	static int cNumHairs; // Number of all hairs
	static const int cNumParticles = 2; // Number of particles
	static const int cNumHairEdges = cNumParticles-1;
	static String cMaterialName;

	Hair();

	void setRootPos(Vector3& pos);
	Vector3& getRootPos();
	Hair& operator = ( const Hair& rhs );

	static void createHairMaterial();
	void updateHairEdges();
};


#endif