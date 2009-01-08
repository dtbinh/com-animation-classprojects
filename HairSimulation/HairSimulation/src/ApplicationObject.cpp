#include "ApplicationObject.h"
#include "World.h"

//-------------------------------------------------------------------------
ApplicationObject::ApplicationObject(const String& name)
{
	mSceneNode = 0;
	mEntity = 0;
}
//-------------------------------------------------------------------------
ApplicationObject::~ApplicationObject()
{
	SceneManager* sm = World::getSingleton().getSceneManager();
	if (mSceneNode)
	{
		sm->destroySceneNode(mSceneNode->getName());
		mSceneNode = 0;
	}
}
//------------------------------------------------------------------------
void ApplicationObject::setPosition(const Vector3& vec)
{
	setPosition(vec.x, vec.y, vec.z);
}
//-----------------------------------------------------------------------
void ApplicationObject::setPosition(Real x, Real y, Real z)
{
	mSceneNode->setPosition(x, y, z);
}
//------------------------------------------------------------------------
const Vector3& ApplicationObject::getPosition(void)
{
	return mSceneNode->getPosition();
}
//-------------------------------------------------------------------------
SceneNode* ApplicationObject::getSceneNode(void)
{
    return mSceneNode;
}
//-------------------------------------------------------------------------
Entity* ApplicationObject::getEntity(void)
{
    return mEntity;
}
