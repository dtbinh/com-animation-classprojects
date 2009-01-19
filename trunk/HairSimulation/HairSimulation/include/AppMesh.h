#ifndef _APP_MESH_H_
#define _APP_MESH_H_

/*
	Simple mesh class
	---------------------------------------------------------
	Contain vertex and index information for meshes in Ogre
*/
#include "DynamicLines.h"
#include <Ogre.h>
using namespace Ogre;

class CMesh
{
public:
	enum SelectionModes{ SELECT_ADD, SELECT_SUB };
	enum TriFlags{ TF_SELECTED = 1, TF_BACKFACING = (1<<1) };
	static int cMeshCount;

	CMesh(SceneNode* sceneNode, Entity* entity);
	~CMesh();
	void generateMeshInfo(void);
	void calculateFaceNormals(void);
	void renderAllMesh(void);
	void renderSelectedFaces(void);
	
	void clearSelection();
	void setSelectionMode( int mode );
	void selectTriangle( int numTriangle );
	void markBackfacing(const Vector3 &camPos);
	
	int LineSelect( const Vector3 &p1, const Vector3 &p2 );
	int FrustumSelect( Vector3 normals[4], Vector3 points[8] );


	//------------------------------------------------------------------------------
	size_t getVertexCount(void);
	size_t getIndexCount(void);
	Vector3* getVertices(void);
	Vector3* getNormals(void);
	unsigned long* getIndices(void);
	int* getTriFlags(void);
	size_t getTriCount(void);
	DynamicLines* getVisualMesh();

private:
	
	// Member Data		
	SceneNode		*mSceneNode, *mMeshSceneNode;					// Scene node of this mesh
	Entity*			mEntity;					// Entity of this mesh
	size_t			mVertexCount, mIndexCount;	// Vertex count and index count
	size_t			mTriCount;					// Triangle count
	Vector3*		mVertices;					//	All vertices
	unsigned long*	mIndices;					//	All indices of vertices
	Vector3*		mNormals;					//	Face normals
	int*			mTriFlags;					//	Selection flag for triangle
	int				mSelectionMode;				//	Selection mode
	DynamicLines	*mVisualMesh;
	ManualObject *mSelectedFaces;
	
};

#endif