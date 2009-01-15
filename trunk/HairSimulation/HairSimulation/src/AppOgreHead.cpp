#include "AppOgreHead.h"
#include "World.h"

//-------------------------------------------------------------------------
OgreHead::OgreHead(const String& name) : ApplicationObject(name)
{
    setUp(name);
}
//-------------------------------------------------------------------------
OgreHead::~OgreHead()
{
}
//-------------------------------------------------------------------------
void OgreHead::setUp(const String& name)
{
    // Create visual presence
    SceneManager* sm = World::getSingleton().getSceneManager();
    mEntity = sm->createEntity(name, "OgreHead.mesh");
    mSceneNode = sm->getRootSceneNode()->createChildSceneNode(name);
    mSceneNode->attachObject(mEntity);
    // Add reverse reference
    mEntity->setUserObject(this);

	//Generate mesh information
	setupMesh();

}

