#include "AppManHead.h"
#include "World.h"

//-------------------------------------------------------------------------
ManHead::ManHead(const String& name) : ApplicationObject(name)
{
    setUp(name);
}
//-------------------------------------------------------------------------
ManHead::~ManHead()
{
}
//-------------------------------------------------------------------------
void ManHead::setUp(const String& name)
{
    // Create visual presence
    SceneManager* sm = World::getSingleton().getSceneManager();
    mEntity = sm->createEntity(name, "facial.mesh");
    mSceneNode = sm->getRootSceneNode()->createChildSceneNode(name);
    mSceneNode->attachObject(mEntity);
	
	Real scale = Real(0.5f);
	mSceneNode->scale(scale, scale, scale);

    // Add reverse reference
    mEntity->setUserObject(this);

	//Generate mesh information
	setupMesh();
}