#ifndef _DYNAMICS_BOX_H_
#define _DYNAMICS_BOX_H_

#include "AppPrerequisites.h"
#include "World.h"
#include "DynamicLInes.h"

//~~~~~~~~~ Bounding Box ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class BoundBox{
public:
	static int mWiredBoxCount;	//	Count of wired boxes to be drawn
	DynamicLines* mWiredBox;
	static SceneNode *mWiredBoxNode;

	Vector3 center;
	Vector3 dim;		// dimension of this box. dim = (max - min)/2
	Vector3 bounds[2];	// { min, max }

	BoundBox();
	~BoundBox();
	inline void display() const;
	inline int longestAxis() const;
	inline bool BallBoxIntersect( const Vector3 & C, float R2 ) const;
	inline bool boxBoxIntersect( const BoundBox & B ) const;
	
};



#endif