#include "Hair.h"

int Hair::cNumHairs = 0;
String Hair::cMaterialName = "HairMaterial";
const float Hair::HAIR_THICKNESS = 0.1;
const float Hair::HAIR_EDGE_LENGTH = 2;

Hair::Hair()
{
	mInterpolated = false;
	memset(mNeighbors, 0, sizeof(int)*3);
	mParticlePoses.reserve(cNumParticles);
	// Setup visual components
	

	mHairEdges = new AppBox[cNumHairEdges];
	
	for (int i = 0; i < cNumHairEdges; i++)
	{
		mHairEdges[i].setMaterialName("1 - Default");
	}
}

//---------------------------------------------
Hair::~Hair()
{
	delete[] mHairEdges;
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
	mParticlePoses = rhs.mParticlePoses;

	
	return *this;
}

//---------------------------------------------
void Hair::createHairMaterial()
{
  MaterialPtr materialPtr = MaterialManager::getSingleton().create(cMaterialName,
              ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  materialPtr->setAmbient(ColourValue(1, 0.84, 0));	// Brown
  materialPtr->setDiffuse(ColourValue(1, 0.84, 0));			// Gold
  materialPtr->setSpecular(ColourValue(1, 0.84, 0));	// Brown

}

//---------------------------------------------
void Hair::updateHairEdges()
{
	for (int i = 0; i < cNumHairEdges; i++)
	{
		mHairEdges[i].setPositionByEnds(mParticlePoses[i], mParticlePoses[i+1]);
	}
}

//----------------------------------------------
void Hair::updateHairEdgesImproved()
{
	for (int i = 0; i < cNumHairEdges; i++)
	{
		//mHairEdges[i].setPositionByEnds(mParticlePoses[i], mParticlePoses[i+1]);
		//std::cout << "i=" << i << ", sizeof (*mParticles)=" << (*mParticles).size() << std::endl;
		mHairEdges[i].setPositionByEnds((*mParticles)[i].x, (*mParticles)[i+1].x);
	}
}

//---------------------------------------------
void Hair::initParticlePoses(Vector3& normal)
{
	Vector3 *previous;

	mParticlePoses.push_back(mRootPos);
	previous = &mParticlePoses[0];
	for (int i = 1; i < cNumParticles; i++)
	{
		mParticlePoses.push_back( (*previous) + normal * HAIR_EDGE_LENGTH );
		previous = &mParticlePoses[i];
	}
}

//---------------------------------------------
size_t Hair::getNumParticles()
{
	return mParticlePoses.size();
}