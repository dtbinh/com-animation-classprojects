#include "Dynamics_box.h"

int BoundBox::mWiredBoxCount = 0;
SceneNode* BoundBox::mWiredBoxNode = NULL;

BoundBox::BoundBox()
{
	// Allocate mWiredBox
	if (mWiredBoxNode == NULL)
		mWiredBoxNode = World::getSingleton().getSceneManager()->getRootSceneNode()->createChildSceneNode();
	mWiredBox = new DynamicLines(ColourValue::White, RenderOperation::OT_LINE_LIST);	// White color
	mWiredBoxNode->attachObject(mWiredBox);
}

BoundBox::~BoundBox()
{
	mWiredBoxNode->detachObject(mWiredBox);
	delete mWiredBox;
}


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
	Real C0plusD0, C1plusD1, C2plusD2;
	Real C0minusD0, C1minusD1, C2minusD2;

	C0plusD0 = center[0] +dim[0];
	C1plusD1 = center[1] +dim[1];
	C2plusD2 = center[2] +dim[2];

	C0minusD0 = center[0] -dim[0];
	C1minusD1 = center[1] -dim[1];
	C2minusD2 = center[2] -dim[2];
	// Reset mWiredBox
	mWiredBox->clear();
	//----------------------------------------Face 1
	mWiredBox->addPoint( C0plusD0, C1plusD1, C2plusD2 );
	mWiredBox->addPoint( C0plusD0, C1minusD1, C2plusD2 );

	mWiredBox->addPoint( C0plusD0, C1minusD1, C2plusD2 );
	mWiredBox->addPoint( C0plusD0, C1minusD1, C2minusD2 );

	mWiredBox->addPoint( C0plusD0, C1minusD1, C2minusD2 );
	mWiredBox->addPoint( C0plusD0, C1plusD1, C2minusD2 );

	mWiredBox->addPoint( C0plusD0, C1plusD1, C2minusD2 );
	mWiredBox->addPoint( C0plusD0, C1plusD1, C2plusD2 );

	//----------------------------------------Face 2
	mWiredBox->addPoint( C0minusD0, C1plusD1, C2plusD2 );
	mWiredBox->addPoint( C0minusD0, C1minusD1, C2plusD2 );
	
	mWiredBox->addPoint( C0minusD0, C1minusD1, C2plusD2 );
	mWiredBox->addPoint( C0minusD0, C1minusD1, C2minusD2 );

	mWiredBox->addPoint( C0minusD0, C1minusD1, C2minusD2 );
	mWiredBox->addPoint( C0minusD0, C1plusD1, C2minusD2 );

	mWiredBox->addPoint( C0minusD0, C1plusD1, C2minusD2 );
	mWiredBox->addPoint( C0minusD0, C1plusD1, C2plusD2 );

	//----------------------------------------- Face 3
	mWiredBox->addPoint( C0plusD0, C1plusD1, C2plusD2 );
	mWiredBox->addPoint( C0minusD0, C1plusD1, C2plusD2 );

	mWiredBox->addPoint( C0minusD0, C1plusD1, C2plusD2 );
	mWiredBox->addPoint( C0minusD0, C1plusD1, C2minusD2 );

	mWiredBox->addPoint( C0minusD0, C1plusD1, C2minusD2 );
	mWiredBox->addPoint( C0plusD0, C1plusD1, C2minusD2 );

	mWiredBox->addPoint( C0plusD0, C1plusD1, C2minusD2 );
	mWiredBox->addPoint( C0plusD0, C1plusD1, C2plusD2 );

	//------------------------------------------Face 4
	mWiredBox->addPoint( C0plusD0, C1minusD1, C2plusD2 );
	mWiredBox->addPoint( C0minusD0, C1minusD1, C2plusD2 );

	mWiredBox->addPoint( C0minusD0, C1minusD1, C2plusD2 );
	mWiredBox->addPoint( C0minusD0, C1minusD1, C2minusD2 );

	mWiredBox->addPoint( C0minusD0, C1minusD1, C2minusD2 );
	mWiredBox->addPoint( C0plusD0, C1minusD1, C2minusD2 );

	mWiredBox->addPoint( C0plusD0, C1minusD1, C2minusD2 );
	mWiredBox->addPoint( C0plusD0, C1minusD1, C2plusD2 );

	//-------------------------------------------Face 5
	mWiredBox->addPoint( C0plusD0, C1plusD1, C2plusD2 );
	mWiredBox->addPoint( C0minusD0, C1plusD1, C2plusD2 );

	mWiredBox->addPoint( C0minusD0, C1plusD1, C2plusD2 );
	mWiredBox->addPoint( C0minusD0, C1minusD1, C2plusD2 );

	mWiredBox->addPoint( C0minusD0, C1minusD1, C2plusD2 );
	mWiredBox->addPoint( C0plusD0, C1minusD1, C2plusD2 );

	mWiredBox->addPoint( C0plusD0, C1minusD1, C2plusD2 );
	mWiredBox->addPoint( C0plusD0, C1plusD1, C2plusD2 );

	//------------------------------------------Face 6
	mWiredBox->addPoint( C0plusD0, C1plusD1, C2minusD2 );
	mWiredBox->addPoint( C0minusD0, C1plusD1, C2minusD2 );

	mWiredBox->addPoint( C0minusD0, C1plusD1, C2minusD2 );
	mWiredBox->addPoint( C0minusD0, C1minusD1, C2minusD2 );

	mWiredBox->addPoint( C0minusD0, C1minusD1, C2minusD2 );
	mWiredBox->addPoint( C0plusD0, C1minusD1, C2minusD2 );

	mWiredBox->addPoint( C0plusD0, C1minusD1, C2minusD2 );
	mWiredBox->addPoint( C0plusD0, C1plusD1, C2minusD2 );
	
	mWiredBoxNode->attachObject(mWiredBox);
}
