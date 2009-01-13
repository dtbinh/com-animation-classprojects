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
#include <OgreOpcode.h>

using namespace Ogre;
using namespace OgreOpcode;

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

	  //	CollisionContext for coliision detection
	 CollisionContext* mCollisionContext;

	 //	Contact vector for ray collision detection
	 std::vector<Vector3> mContactVector;

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

	/** Clears the scene */
	void clear(void);

	/** Override standard Singleton retrieval */
	static World& getSingleton(void);
	static World* getSingletonPtr(void);
	

//----------------- Head creation methods ------------------------------------------------------------------
	/**	Create a ball object */
	Ball* createBall(const String& name, Real radius, const Vector3& pos = Vector3::ZERO);
	/** Create an OGRE head object. */
    OgreHead* createOgreHead(const String& name, const Vector3& pos = Vector3::ZERO);
	/** Create a man head object. */
    ManHead* createManHead(const String& name, const Vector3& pos = Vector3::ZERO);

//----------------- Methods for scalp and hair roots -----------------------------------------------------
	/** Create the scalp */
	void createScalpCircle(const String& name);
	/** Complete the DynamicLines as scalp circle */
	void completeScalpCircle(void);
	//	Add a point to the scalp circle
	void addPointToScalpCircle(const Vector3& point);
	/** Generate hair roots */
	void generateHairRoots(void);

//---------------- Methods related to collision detection ------------------------------------------------
	/** Add an entity to CollisionContext */
	void addCollisionEntity(Entity* entity);
	/** Remove an entity from CollisionContext */
	void removeCollisionEntity(Entity* entity);
	/** Test for ray collision */
	void doRayTest(const Ray &ray, const Real MaxDistance = 2000.0f);
	/** Get the first point in mContactVector */
	Vector3* getContactPoint(void);


};


#endif