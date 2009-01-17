#include "Hair.h"

int Hair::cNumHairs = 0;

Hair::Hair()
{
	mInterpolated = false;
	memset(mNeighbors, 0, sizeof(int)*3);
}

//---------------------------------------------
void Hair::setRootPos(Vector3& pos)
{
	mRootPos = pos;
}

//---------------------------------------------
Vector3& Hair::getRootPos()
{
	return mRootPos; 
}

//---------------------------------------------
Hair& Hair::operator =(const Hair &rhs)
{
	mRootPos = rhs.mRootPos;

	return *this;
}