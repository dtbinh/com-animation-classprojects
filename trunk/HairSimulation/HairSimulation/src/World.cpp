
#include "World.h"
#include "AppUtilities.h"

using namespace std;

float World::TRI_AREA_THRESHOLD = 0.5;

//-----------------------------------------------------------
World::World(SceneManager* sceneMgr)
: mSceneMgr(sceneMgr)
{
	mProcessState = PS_INITIAL;
	mAllHairs = 0;

	// Create a CollisionContext
	//mCollisionContext = CollisionManager::getSingletonPtr()->createContext("mCollisionContext");
}
//-----------------------------------------------------------
World::~World()
{
	clear();
}
//-----------------------------------------------------------
SceneManager* World::getSceneManager(void)
{
	return mSceneMgr;
}
//-----------------------------------------------------------
void World::clear(void)
{
	
	ObjectMap::iterator i;
	for (i = mObjects.begin(); i != mObjects.end(); ++i)
	{
		delete i->second;
	}
	mObjects.clear();
}
//-------------------------------------------------------------------------
template<> World* Ogre::Singleton<World>::ms_Singleton = 0;
World* World::getSingletonPtr(void)
{
    return ms_Singleton;
}
World& World::getSingleton(void)
{  
    assert( ms_Singleton );  return ( *ms_Singleton );  
}
//-------------------------------------------------------------------------
Ball* World::createBall(const Ogre::String &name, Ogre::Real radius, const Ogre::Vector3 &pos)
{
	Ball* ball = new Ball(name, radius);

	ball->setPosition(pos);

	mObjects[name] = ball;

	//addCollisionEntity(ball->getEntity());
	return ball;
}
//-------------------------------------------------------------------------
OgreHead* World::createOgreHead(const String& name, const Vector3& pos)
{
    OgreHead* head = new OgreHead(name);

    head->setPosition(pos);

    mObjects[name] = head;

	//addCollisionEntity(head->getEntity());
    return head;
}
//-------------------------------------------------------------------------
ManHead* World::createManHead(const String& name, const Vector3& pos)
{
    ManHead* head = new ManHead(name);

    head->setPosition(pos);

    mObjects[name] = head;

	//addCollisionEntity(head->getEntity());
    return head;
}

//-------------------------------------------------------------------------
World::ProcessState World::getProcessState(void)
{
	return mProcessState;
}

//-------------------------------------------------------------------------
void World::setProcessState(World::ProcessState ps)
{
	mProcessState = ps;
}

//-------------------------------------------------------------------------
void World::addCollisionEntity( Entity* pEntity )
{
	EntityCollisionShape* pCollisionShape = CollisionManager::getSingletonPtr()->createEntityCollisionShape( pEntity->getName() );
	pCollisionShape->load( pEntity );
	CollisionObject* pCollisionObject = mCollisionContext->createObject( pEntity->getName() );
	pCollisionObject->setCollClass( "mCollClass" );
	pCollisionObject->setShape( pCollisionShape );
	mCollisionContext->addObject( pCollisionObject );
}

//-------------------------------------------------------------------------
void  World::removeCollisionEntity( Ogre::Entity* pEntity )
{
	ICollisionShape* pShape = CollisionManager::getSingletonPtr()->getShape( pEntity->getName() );
	if( NULL != pShape )
	{
		CollisionManager::getSingletonPtr()->destroyShape( pShape );
	}

	CollisionObject* pObject = NULL;
	try
	{
		pObject = mCollisionContext->getAttachedObject( pEntity->getName() );
	}
	catch( ... )
	{
		return;
	}

	mCollisionContext->destroyObject( pObject );
}

//-------------------------------------------------------------------------
void World::doRayTest(const Ogre::Ray &ray, const Ogre::Real MaxDistance)
{
	int ContactCount = 0;
	CollisionPair** ppCollisionPair = NULL;

	mCollisionContext->reset();

	if ((ContactCount = mCollisionContext->rayCheck(ray, MaxDistance, COLLTYPE_EXACT, COLLTYPE_ALWAYS_EXACT, ppCollisionPair)) > 0)
	{
		mContactVector.clear();
		for (int i = 0; i < ContactCount; i++)
		{
			mContactVector.push_back(ppCollisionPair[i]->contact);
		}
	}
}

//-------------------------------------------------------------------------
Vector3* World::getContactPoint(void)
{
	if (mContactVector.size() > 0)
		return &mContactVector[0];
	else
		return NULL;
}

//-------------------------------------------------------------------------
void World::generateHairRoots(CMesh *mesh)
{
	int fIndex, vIndex;
	int fCount;
	int* triFlags;
	unsigned long* indices;
	Vector3* vertices;
	Vector3 *p1, *p2, *p3;
	map<Vector3*, Vector3*> vertexMap;
	typedef pair<Vector3*, Vector3*> VertexPair;
	int rootCount = 0;

	float curArea, maxArea = -1, minArea = 99999;

	// Retrieve the mesh data
	fCount = (int)mesh->getTriCount();
	triFlags = mesh->getTriFlags();
	indices = mesh->getIndices();
	vertices = mesh->getVertices();

	//	Calculate the number of hair roots
	for (fIndex = 0; fIndex < fCount; fIndex++)
	{
		if ((triFlags[fIndex] & CMesh::TF_SELECTED) &&
			!(triFlags[fIndex] & CMesh::TF_BACKFACING))
		{
			vIndex = fIndex*3;
			p1 = &vertices[indices[vIndex]];
			p2 = &vertices[indices[vIndex+1]];
			p3 = &vertices[indices[vIndex+2]];
		
			vertexMap.insert(VertexPair(p1, p1));
			vertexMap.insert(VertexPair(p2, p2));
			vertexMap.insert(VertexPair(p3, p3));
		}
	}

	Hair::cNumHairs = (int)vertexMap.size();
	// Allocate mAllHairs
	mAllHairs = new Hair[Hair::cNumHairs];

	// Setup hair information
	int hIndex = 0;
	vertexMap.clear();
	for (fIndex = 0; fIndex < fCount; fIndex++)
	{
		if ((triFlags[fIndex] & CMesh::TF_SELECTED) &&
			!(triFlags[fIndex] & CMesh::TF_BACKFACING))
		{
			vIndex = fIndex*3;
			p1 = &vertices[indices[vIndex]];
			p2 = &vertices[indices[vIndex+1]];
			p3 = &vertices[indices[vIndex+2]];
		
			// Set root position
			if (vertexMap.insert(VertexPair(p1, p1)).second)
			{
				mAllHairs[hIndex].setRootPos(*p1);
				hIndex++;
			}
			if (vertexMap.insert(VertexPair(p2, p2)).second)
			{
				mAllHairs[hIndex].setRootPos(*p2);
				hIndex++;
			}
			if (vertexMap.insert(VertexPair(p3, p3)).second)
			{
				mAllHairs[hIndex].setRootPos(*p3);
				hIndex++;
			}
			curArea = Utilities::getArea(*p1, *p2, *p3);

			//if (curArea > TRI_AREA_THRESHOLD)
			//	generateHairRootsInsideTri(*p1, *p2, *p3, vertexMap, curArea);
			// Statistics of triangle area
			if (curArea > maxArea)
				maxArea = curArea;
			if (curArea < minArea)
				minArea = curArea;
		}
	}

	cout << "minArea = " << minArea << ", maxArea = " << maxArea << endl;

	//	Render hair roots
	ManualObject* rootPoints = mSceneMgr->createManualObject("HairRoots");
	rootPoints->begin("BaseWhiteNoLighting", RenderOperation::OT_POINT_LIST);

	for (int i=0; i<Hair::cNumHairs; i++)
	{
		rootPoints->position(mAllHairs[i].getRootPos());
	}
	rootPoints->end();
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(rootPoints);
}

//----------------------------------------------------------------------------------------------------------------------------------------------
void World::generateHairRootsInsideTri(Ogre::Vector3 &p1, Ogre::Vector3 &p2, Ogre::Vector3 &p3, map<Vector3*, Vector3*>& vertexMap, float area)
{
	Vector3 midP1, midP2, midP3;
	// Extend the size of mAllHairs
	Hair *tempHairs = new Hair[Hair::cNumHairs+3];

	for (int i = 0; i < Hair::cNumHairs; i++)
	{
		tempHairs[i] = mAllHairs[i];
	}
	delete[] mAllHairs;
	mAllHairs = tempHairs;
	Hair::cNumHairs += 3;

	// Add 3 new roots
	midP1 = p1.midPoint(p2);
	midP2 = p2.midPoint(p3);
	midP3 = p3.midPoint(p1);
	mAllHairs[Hair::cNumHairs-3].setRootPos(midP1);
	mAllHairs[Hair::cNumHairs-2].setRootPos(midP2);
	mAllHairs[Hair::cNumHairs-1].setRootPos(midP3);

	// Recursively call self if the area is big enough
	if ((area / 4.0) > TRI_AREA_THRESHOLD)
	{
		//generateHairRootsInsideTri(p1, midP1, midP3
	}
}

//-------------------------------------------------------------------------
//	Setup CollisionManager
void World::setupCollisionManager()
{
	//	Create a CollisionManager and choose a SceneManager
	mCollisionMgr = new CollisionManager(mSceneMgr);
	CollisionManager::getSingletonPtr()->setSceneManager(mSceneMgr);
	//	Add collision class and collision type
	CollisionManager::getSingletonPtr()->addCollClass("mCollClass");
	CollisionManager::getSingletonPtr()->addCollType("mCollClass", "mCollClass", COLLTYPE_EXACT);
}

//-------------------------------------------------------------------------
void World::handleProcessState()
{
	switch(mProcessState)
	{
	case PS_INITIAL:
		break;

	case PS_SELECT_SCALP:
		break;

	case PS_GENERATION:
		break;

	case PS_SIMULATION:
		break;
	}
}