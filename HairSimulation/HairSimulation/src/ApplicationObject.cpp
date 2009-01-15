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
void ApplicationObject::setupMesh(void)
{
	if ((mSceneNode == NULL) || (mEntity == NULL))
	{
		std::cout << "Error in ApplicationObject::setupMesh()" << std::endl;
		return;
	}

	mMesh = new CMesh(mSceneNode, mEntity);
	mMesh->generateMeshInfo();
	mVisualMesh = new DynamicLines(ColourValue::White,
		RenderOperation::OT_LINE_LIST);
}

//-------------------------------------------------------------------------
void ApplicationObject::createVisualMesh()
{
	int fIndex, vIndex;
	int fCount = (int)mMesh->getTriCount();
	Vector3* vertices = mMesh->getVertices();
	unsigned long* indices = mMesh->getIndices();
	Vector3 p1, p2, p3;
	
	// For each triangle, add three lines
	for (fIndex = 0; fIndex < fCount; fIndex++)
	{
		vIndex = fIndex*3;
		p1 = vertices[indices[vIndex]];
		p2 = vertices[indices[vIndex+1]];
		p3 = vertices[indices[vIndex+2]];
		// Line 1
		mVisualMesh->addPoint(p1);
		mVisualMesh->addPoint(p2);
		// Line 2
		mVisualMesh->addPoint(p1);
		mVisualMesh->addPoint(p3);
		// Line 3
		mVisualMesh->addPoint(p2);
		mVisualMesh->addPoint(p3);
	}
	mVisualMesh->update();
}	

//-------------------------------------------------------------------------
void ApplicationObject::attachVisualMesh()
{
	mEntity->setVisible(false);
	mSceneNode->attachObject(mVisualMesh);
}