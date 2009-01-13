#include "ApplicationObject.h"
#include "World.h"
#include "AppUtilities.h"

//-------------------------------------------------------------------------
ApplicationObject::ApplicationObject(const String& name)
{
	mSceneNode = 0;
	mEntity = 0;
	mIndexCount = 0;
	mIndices = 0;
	mVertexCount = 0;
	mVertices = 0;
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
size_t ApplicationObject::getVertexCount(void)
{
	return mVertexCount;
}

//-------------------------------------------------------------------------
size_t ApplicationObject::getIndexCount(void)
{
	return mIndexCount;
}

//-------------------------------------------------------------------------
Vector3* ApplicationObject::getVertices(void)
{
	return mVertices;
}

//-------------------------------------------------------------------------
unsigned long* ApplicationObject::getIndices(void)
{
	return mIndices;
}

//-------------------------------------------------------------------------
void ApplicationObject::generateMeshInfo(void)
{
	if ((mSceneNode == NULL) || (mEntity == NULL))
	{
		std::cout << "Error in ApplicationObject::generateMeshInfo()" << std::endl;
		return;
	}

	Utilities::getMeshInformation(mEntity->getMesh(), mVertexCount, mVertices, mIndexCount, mIndices,
		mSceneNode->getWorldPosition(), mSceneNode->getWorldOrientation(), mSceneNode->getScale());
}

//-------------------------------------------------------------------------
void ApplicationObject::clearMeshInfo(void)
{
	delete[] mVertices;
	delete[] mIndices;
}
