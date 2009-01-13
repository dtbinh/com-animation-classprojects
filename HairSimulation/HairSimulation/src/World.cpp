
#include "World.h"

//-----------------------------------------------------------
World::World(SceneManager* sceneMgr)
: mSceneMgr(sceneMgr)
{
	mProcessState = PS_INITIAL;

	// Create a CollisionContext
	mCollisionContext = CollisionManager::getSingletonPtr()->createContext("mCollisionContext");
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
	ball->generateMeshInfo();
	mObjects[name] = ball;

	addCollisionEntity(ball->getEntity());
	return ball;
}
//-------------------------------------------------------------------------
OgreHead* World::createOgreHead(const String& name, const Vector3& pos)
{
    OgreHead* head = new OgreHead(name);

    head->setPosition(pos);
	head->generateMeshInfo();
    mObjects[name] = head;

	addCollisionEntity(head->getEntity());
    return head;
}
//-------------------------------------------------------------------------
ManHead* World::createManHead(const String& name, const Vector3& pos)
{
    ManHead* head = new ManHead(name);

    head->setPosition(pos);
	head->generateMeshInfo();
    mObjects[name] = head;

	addCollisionEntity(head->getEntity());
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
void World::createScalpCircle(const String& name)
{
	mScalpCircle = new DynamicLines(RenderOperation::OT_LINE_STRIP);

	SceneManager* sm = World::getSingleton().getSceneManager();

	SceneNode *scalpCircleNode = sm->getRootSceneNode()->createChildSceneNode(name);
	scalpCircleNode->attachObject(mScalpCircle);

}

//-------------------------------------------------------------------------
void World::addPointToScalpCircle(const Vector3& point)
{
	mScalpCircle->addPoint(point);
	mScalpCircle->update();
	//	Update the SceneNode to redraw DynamicLines
	mScalpCircle->getParentSceneNode()->needUpdate(true);

}
//-------------------------------------------------------------------------
void World::completeScalpCircle()
{
	int numPoints = (int) mScalpCircle->getNumPoints();
	if (numPoints < 3)
	{
		std::cout << "DynamicLines for scalp circle is too short" << std::endl;
		exit(1);
	}
	//	Add the starting point to the end of DynamicLines to complete the circle
	addPointToScalpCircle(mScalpCircle->getPoint(0));
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
void World::generateHairRoots(void)
{


}

