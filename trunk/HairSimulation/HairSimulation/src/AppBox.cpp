#include "AppBox.h"
#include "World.h"
#include "Hair.h"

int AppBox::cNumAppBox = 0;

//--------------------------------------------------------------------
AppBox::AppBox() : ApplicationObject()
{
	mDimensions.x = Hair::HAIR_THICKNESS;
	mDimensions.y = Hair::HAIR_THICKNESS;
	mDimensions.z = Hair::HAIR_EDGE_LENGTH;
	String name = "AppBox" + StringConverter::toString(cNumAppBox++);
	setUp(name);
}

//--------------------------------------------------------------------
AppBox::AppBox(const Ogre::String &name, Real width, Real height, Real depth) : ApplicationObject(name)
{
	mDimensions.x = width;
	mDimensions.y = height;
	mDimensions.z = depth;
	setUp(name);
}
//--------------------------------------------------------------------
AppBox::~AppBox()
{
}
//--------------------------------------------------------------------
void AppBox::setUp(const Ogre::String &name)
{
	//	Create visual presence
	SceneManager* sm = World::getSingleton().getSceneManager();
	mEntity = sm->createEntity(name, "cube.mesh");

	mSceneNode = sm->getRootSceneNode()->createChildSceneNode(name);
	//	Scale down, default size is 100x100x100
	mSceneNode->scale(mDimensions.x / 100.0f,
		mDimensions.y / 100.0f,
		mDimensions.z / 100.0f);

	mSceneNode->attachObject(mEntity);
	//	Add reverse reference
	mEntity->setUserObject(this);

}

//--------------------------------------------------------------------
void AppBox::setMaterialName(const String& matName)
{
	mEntity->setMaterialName(matName);
}

//--------------------------------------------------------------------
void AppBox::setPositionByEnds(const Vector3 &a, const Vector3 &b)
{
	mSceneNode->setPosition(a.midPoint(b));
	//mSceneNode->resetOrientation();
	mDimensions.z = a.distance(b);
	mSceneNode->setScale(mDimensions.x / 100.0,
						 mDimensions.y / 100.0,
						 mDimensions.z / 100.0);
	mSceneNode->setDirection(b-a);
	
	
}
#include "Dynamics_box.h"