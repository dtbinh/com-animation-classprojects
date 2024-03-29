#ifndef _APPLICATIONOBJECT_H_
#define _APPLICATIONOBJECT_H_

#include "AppMesh.h"
#include "DynamicLines.h"
#include <Ogre.h>

using namespace Ogre;

class ApplicationObject : public UserDefinedObject
{
protected:
	//		Visual component
	SceneNode *mSceneNode;
	Entity* mEntity;
	CMesh* mMesh;	// Mesh data (vertices, indices, and normals)

	//		Set up method, must override
	virtual void setUp(const String& name) = 0;

public:
		ApplicationObject();
		ApplicationObject(const String& name);
		virtual ~ApplicationObject();

		/** Sets the position of this object. */
		virtual void setPosition(const Vector3& vec);
		/** Sets the position of this object. */
		virtual void setPosition(Real x, Real y, Real z);
		/** Gets the current position of this object. */
		virtual const Vector3& getPosition(void);

		/** Gets the SceneNode which is being used to represent this object's position in 
            the OGRE world. */
        SceneNode* getSceneNode(void);
        /** Gets the Entity which is being used to represent this object in the OGRE world. */
        Entity* getEntity(void);

		CMesh* getMesh(void);
		void deleteMesh(void);

		/** Setup mesh data */
		void setupMesh(void);
			
		void attachVisualMesh(void);
};

#endif