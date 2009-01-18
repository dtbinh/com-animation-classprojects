
#include "World.h"
#include "AppUtilities.h"

using namespace std;

float World::TRI_AREA_THRESHOLD = 9999.0;

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
void World::generateHairs(CMesh *mesh)
{
	int fIndex, vIndex;
	int fCount;
	int* triFlags;
	unsigned long* indices;
	Vector3* vertices;
	Vector3 *p1, *p2, *p3;
	map<Vector3*, Vector3*> vertexMap;
	vector<Vector3> inTriVertexList;
	typedef pair<Vector3*, Vector3*> VertexPair;
	int rootCount = 0;
	Vector3* normals;
	Vector3 fNormal;

	float curArea, maxArea = -1, minArea = 99999;

	// Retrieve the mesh data
	fCount = (int)mesh->getTriCount();
	triFlags = mesh->getTriFlags();
	indices = mesh->getIndices();
	vertices = mesh->getVertices();
	normals = mesh->getNormals();

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
	// Set the hair material
	//Hair::createHairMaterial();
	// Allocate mAllHairs
	mAllHairs = new Hair[Hair::cNumHairs];

	// Setup hair information and insert new roots if too sparse
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
			fNormal = normals[fIndex];
		
			// Set root position
			if (vertexMap.insert(VertexPair(p1, p1)).second)
			{
				mAllHairs[hIndex].setRootPos(*p1);
				mAllHairs[hIndex].initParticlePoses(fNormal);
				hIndex++;
			}
			if (vertexMap.insert(VertexPair(p2, p2)).second)
			{
				mAllHairs[hIndex].setRootPos(*p2);
				mAllHairs[hIndex].initParticlePoses(fNormal);
				hIndex++;
			}
			if (vertexMap.insert(VertexPair(p3, p3)).second)
			{
				mAllHairs[hIndex].setRootPos(*p3);
				mAllHairs[hIndex].initParticlePoses(fNormal);
				hIndex++;
			}
			// If face area is larger than the threshold, create new roots
			curArea = Utilities::getArea(*p1, *p2, *p3);
			if (curArea > TRI_AREA_THRESHOLD)
				generateHairsInsideTri(*p1, *p2, *p3, inTriVertexList, curArea, fNormal);
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
void World::generateHairsInsideTri(Ogre::Vector3 &p1, Ogre::Vector3 &p2, Ogre::Vector3 &p3, vector<Vector3>& vertexList, float area, Vector3& normal)
{
	Vector3 midP1, midP2, midP3;
	Hair *tempHairs;
	int newVertexCount = 0;
	Vector3 newVertices[3];

	// Get midpoints
	midP1 = p1.midPoint(p2);
	midP2 = p2.midPoint(p3);
	midP3 = p3.midPoint(p1);

	// Check if each midpoint is repeated
	if (!Utilities::isInList(midP1, vertexList))
	{
		vertexList.push_back(midP1);
		newVertices[newVertexCount] = midP1;
		newVertexCount++;
	}
	if (!Utilities::isInList(midP2, vertexList))
	{
		vertexList.push_back(midP2);
		newVertices[newVertexCount] = midP2;
		newVertexCount++;
	}
	if (!Utilities::isInList(midP3, vertexList))
	{
		vertexList.push_back(midP3);
		newVertices[newVertexCount] = midP3;
		newVertexCount++;
	}
//cout << "newVertexCount=" << newVertexCount << endl;

	if (newVertexCount != 0)
	{
		// Extend the size of mAllHairs
		tempHairs = new Hair[Hair::cNumHairs + newVertexCount];
		for (int i = 0; i < Hair::cNumHairs; i++)
		{
			tempHairs[i] = mAllHairs[i];
		}
		delete[] mAllHairs;
		mAllHairs = tempHairs;

		// Set 3 new roots
		for (int i = 0; i <newVertexCount; i++)
		{
			mAllHairs[Hair::cNumHairs+i].setRootPos(newVertices[i]);
			mAllHairs[Hair::cNumHairs+i].initParticlePoses(normal);
		}

		Hair::cNumHairs += newVertexCount;
	}

	float oneQuarterArea = area/4.0;
	// Recursively call self if the area is big enough
	if (oneQuarterArea > TRI_AREA_THRESHOLD)
	{
		generateHairsInsideTri(p1, midP1, midP3, vertexList, oneQuarterArea, normal);
		generateHairsInsideTri(midP1, p2, midP2, vertexList, oneQuarterArea, normal);
		generateHairsInsideTri(midP3, midP2, p3, vertexList, oneQuarterArea, normal);
		generateHairsInsideTri(midP1, midP2, midP3, vertexList, oneQuarterArea, normal);
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

//-------------------------------------------------------------------------
void World::updateHairs()
{
	for (int i = 0; i < Hair::cNumHairs; i++)
	{
		mAllHairs[i].updateHairEdges();
	}
}

void World::checkAll()
{
	for (int i = 0; i < Hair::cNumHairs; i++)
	{
		if (mAllHairs[i].getNumParticles() != Hair::cNumParticles)
		{
			cout << "Error of Num particles : mAllHairs[" << i << "].getNumParticles()=" << mAllHairs[i].getNumParticles()  << endl;
			system("PAUSE");
			exit(1);
		}
	}
	cout << "checkAll() passed" << endl;
}