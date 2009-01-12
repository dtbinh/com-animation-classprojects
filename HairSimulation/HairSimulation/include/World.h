/*
-----------------------------------------------------------------------
class World contains all objects required to simulate hair, such as a head and hairs.

-----------------------------------------------------------------------
*/
#ifndef _WORLD_H_
#define _WORLD_H_

#include "ApplicationObject.h"
#include "AppBall.h"
#include "AppOgreHead.h"
#include "AppManHead.h"
#include "DynamicLines.h"
#include <Ogre.h>
#include <OgreSingleton.h>

using namespace Ogre;

class World : public Singleton<World>
{
public:
	//	Process state type
	enum ProcessState {
		PS_INITIAL,
		PS_SELECT_SCALP,
		PS_GENERATION,
		PS_SIMULATION
	};

protected:
	// Pointer to OGRE's scene manager
	 SceneManager* mSceneMgr;

	 typedef std::map<String, ApplicationObject*> ObjectMap;
	//  Main list of objects
	 ObjectMap mObjects;

	 typedef std::set<ApplicationObject*> ObjectSet;
	 //	Set of dynamics objects
	 ObjectSet mDynamicsObjects;

	 //	Process state of hair simulation
	 ProcessState mProcessState;

	 // The pointer to Scalp
	 DynamicLines* mScalpCircle;

public:
	/** Creates an instance of the world */
	World(SceneManager* sceneMgr);
	~World();
	
	//	Get the scene manager for this world
	SceneManager* getSceneManager(void);
	
	//	Get the process state of hair simulation
	ProcessState getProcessState(void);
	//	Set the process state of hair simulaiton
	void setProcessState(ProcessState ps);

	//	Add a point to the scalp circle
	void addPointToScalpCircle(const Vector3& point);

	/**	Create a ball object */
	Ball* createBall(const String& name, Real radius, const Vector3& pos = Vector3::ZERO);

	/** Create an OGRE head object. */
    OgreHead* createOgreHead(const String& name, const Vector3& pos = Vector3::ZERO);

	/** Create a man head object. */
    ManHead* createManHead(const String& name, const Vector3& pos = Vector3::ZERO);

	/** Create the scalp */
	void createScalpCircle(const String& name);

	/** Complete the DynamicLines as scalp circle */
	void completeScalpCircle(void);

	/** Clears the scene */
	void clear(void);

	/** Override standard Singleton retrieval */
	static World& getSingleton(void);
	static World* getSingletonPtr(void);

};


#endif