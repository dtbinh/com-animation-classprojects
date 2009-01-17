#ifndef _HAIR_H_
#define _HAIR_H_

#include <Ogre.h>

using namespace Ogre;

/*
	Hair class
*/
class Hair
{
protected:
	Vector3 mRootPos;		//	Root position
	bool	mInterpolated;	//	Indicate this hair is interpolated from its neighbors
	int		mNeighbors[3];	//	Neighbor hairs(Three at most)

	
	
public:
	static int cNumHairs; // Number of all hairs

	Hair();

	void setRootPos(Vector3& pos);
	Vector3& getRootPos();
	Hair& operator = ( const Hair& rhs );
};


#endif