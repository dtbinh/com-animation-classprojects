#include "Hair.h"

int Hair::cNumHairs = 0;
String Hair::cMaterialName = "HairMaterial";

Hair::Hair()
{
	mInterpolated = false;
	memset(mNeighbors, 0, sizeof(int)*3);
	mParticlePoses.reserve(cNumParticles);
	// Setup visual components
	

	mHairEdges = new AppBox[cNumHairEdges];
	for (int i = 0; i < cNumHairEdges; i++)
	{
		mHairEdges[i].setMaterialName(cMaterialName);
	}
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

//---------------------------------------------
void Hair::createHairMaterial()
{
  MaterialPtr materialPtr = MaterialManager::getSingleton().create(cMaterialName,
              ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  materialPtr->setAmbient(ColourValue(0.65, 0.16, 0.16));	// Brown
  materialPtr->setDiffuse(ColourValue(1, 0.84, 0));			// Gold
  materialPtr->setSpecular(ColourValue(0.65, 0.16, 0.16));	// Brown

}

//---------------------------------------------
void Hair::updateHairEdges()
{
	for (int i = 0; i < cNumHairEdges; i++)
	{
		mHairEdges[i].setPositionByEnds(mParticlePoses[i], mParticlePoses[i+1]);
	}
}