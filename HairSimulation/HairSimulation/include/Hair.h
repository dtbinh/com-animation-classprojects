#ifndef _HAIR_H_
#define _HAIR_H_

#include "AppBox.h"
#include "particle.h"
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
	
	AppBox* mHairEdges;						// Visual components of hair edges
	
	
public:
	std::vector<AppParticle> *mParticles;
	std::vector<Vector3> mParticlePoses;	// Positions of particles

	static const float HAIR_THICKNESS, HAIR_EDGE_LENGTH;
	static const int cNumParticles = 15; // Number of particles
	static const int cNumHairEdges = cNumParticles-1;
	static int cNumHairs; // Number of all hairs
	static String cMaterialName;

	Hair();
	~Hair();

	void setRootPos(Vector3& pos);
	Vector3& getRootPos();
	size_t getNumParticles();
	Hair& operator = ( const Hair& rhs );

	static void createHairMaterial();
	void updateHairEdges();
	void updateHairEdgesImproved();
	/** Initialize mParticlePoses so that all particles are along a line with the direction */
	void initParticlePoses(Vector3& normal);
};


#endif