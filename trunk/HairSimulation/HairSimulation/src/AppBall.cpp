#include "AppBall.h"
#include "World.h"
//--------------------------------------------------------------------
Ball::Ball(const Ogre::String &name, Ogre::Real radius) : ApplicationObject(name)
{
	mRadius = radius;
	setUp(name);
}
//--------------------------------------------------------------------
Ball::~Ball()
{
}
//--------------------------------------------------------------------
void Ball::setUp(const Ogre::String &name)
{
	//	Create visual presence
	SceneManager* sm = World::getSingleton().getSceneManager();
	mEntity = sm->createEntity(name, "sphere.mesh");
	mSceneNode = sm->getRootSceneNode()->createChildSceneNode(name);
	//	Scale down, default size is 100
	Real scale = mRadius / 100.0f;
	mSceneNode->scale(scale, scale, scale);

	mSceneNode->attachObject(mEntity);
	//	Add reverse reference
	mEntity->setUserObject(this);

	//Setup mesh
	setupMesh();
}