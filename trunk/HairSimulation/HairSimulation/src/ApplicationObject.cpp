#include "ApplicationObject.h"
#include "World.h"
#include "AppUtilities.h"

//-------------------------------------------------------------------------
ApplicationObject::ApplicationObject(const String& name)
{
	mSceneNode = 0;
	mEntity = 0;
	mMesh = 0;
	mVisualMesh = 0;
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

//-------------------------------------------------------------------------
CMesh* ApplicationObject::getMesh(void)
{
	return mMesh;
}

//-------------------------------------------------------------------------
void ApplicationObject::setupMesh(void)
{
	if ((mSceneNode == NULL) || (mEntity == NULL))
	{
		std::cout << "Error in ApplicationObject::setupMesh()" << std::endl;
		return;
	}

	mMesh = new CMesh(mSceneNode, mEntity);
	mMesh->generateMeshInfo();
	mMesh->renderAllMesh();
}
