
#include "World.h"

//-----------------------------------------------------------
World::World(SceneManager* sceneMgr)
: mSceneMgr(sceneMgr)
{
	mProcessState = PS_INITIAL;
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

	return ball;
}
//-------------------------------------------------------------------------
OgreHead* World::createOgreHead(const String& name, const Vector3& pos)
{
    OgreHead* head = new OgreHead(name);
    head->setPosition(pos);

    mObjects[name] = head;

    return head;
}
//-------------------------------------------------------------------------
ManHead* World::createManHead(const String& name, const Vector3& pos)
{
    ManHead* head = new ManHead(name);
    head->setPosition(pos);

    mObjects[name] = head;

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