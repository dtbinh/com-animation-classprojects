#ifndef _DYNAMICS_BOX_H_
#define _DYNAMICS_BOX_H_

#include "AppPrerequisites.h"
#include "World.h"
#include "DynamicLInes.h"

//~~~~~~~~~ Bounding Box ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class BoundBox{
public:
	Vector3 center;
	Vector3 dim;		// dimension of this box. dim = (max - min)/2
	Vector3 bounds[2];	// { min, max }
	inline void display() const;
	inline int longestAxis() const;
	inline bool BallBoxIntersect( const Vector3 & C, float R2 ) const;
	inline bool boxBoxIntersect( const BoundBox & B ) const;
	static int mWiredBoxCount;	//	Count of wired boxes to be drawn
	DynamicLines mWiredBoxes;
};

int BoundBox::mWiredBoxCount = 0;
// 找出最長的軸
// 0,1,2 對應 x,y,z
int BoundBox::longestAxis() const
{
	int longest = 0;
	if( dim.x > dim.z ){
		if( dim.x > dim.y ){
			longest = 0;
		}else{
			longest = 1;
		}
	}else{ 
		if( dim.y > dim.z ){
			longest = 1;
		}else{
			longest = 2;
		}
	}
	return longest;
}
//~~~~ box-box intersect ~~~~
bool BoundBox::boxBoxIntersect( const BoundBox & B ) const
{
	for( int i = 0; i < 3; i++ ){
		if( bounds[0][i] > B.bounds[1][i] ||
			B.bounds[0][i] > bounds[1][i] )
		{
			return false;		
		}
	}
	return true;
}

bool BoundBox::BallBoxIntersect(const Vector3 &C, float R2) const
{
	float D = 0;	// distance
	for( int i = 0; i < 3; i++ ){

		if( C[i] < bounds[0][i] ){ // C[i] < min[i]
			float x = C[i] - bounds[0][i];
			D += x * x;
		}else if( C[i] > bounds[1][i] ){
			float x = C[i] - bounds[1][i];
			D += x * x;
		}
	}
	if( D <= R2 ){
		//std::cout << "ball hit box" << std::endl;
		return true;
	}
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//畫出方盒子
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void BoundBox::display() const
{


	SceneNode* sceneNode = World::getSingleton().getSceneManager()->getRootSceneNode()->createChildSceneNode();
	mWiredBoxes = new DynamicLines(ColourValue(), RenderOperation::OT_LINE_STRIP)[6];	// White color
/*
	// Face 1
	mWiredBoxes[0].addPoint( center[0] +dim[0], center[1] +dim[1], center[2] +dim[2] );
	mWiredBoxes[0].addPoint( center[0] +dim[0], center[1] -dim[1], center[2] +dim[2] );
	mWiredBoxes[0].addPoint( center[0] +dim[0], center[1] -dim[1], center[2] -dim[2] );
	mWiredBoxes[0].addPoint( center[0] +dim[0], center[1] +dim[1], center[2] -dim[2] );
	mWiredBoxes[0].addPoint( center[0] +dim[0], center[1] +dim[1], center[2] +dim[2] );

	// Face 2

	// Face 3

	// Face 4

	// Face 5

	// Face 6
	

	sceneNode->attachObject(mWiredBox);*/
}


#endif