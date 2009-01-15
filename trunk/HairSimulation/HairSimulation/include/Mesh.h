#ifndef _MESH_H_
#define _MESH_H_

/*
	Simple mesh class
	---------------------------------------------------------
	Contain vertex and index information for meshes in Ogre
*/
#include <Ogre.h>
using namespace Ogre;

class CMesh
{
public:
	CMesh(SceneNode* sceneNode, Entity* entity);
	~CMesh();
	void generateMeshInfo(void);
	void calculateFaceNormals(void);
	void render(void);
	
	void clearSelection();
	void setSelectionMode( int mode );
	void selectTriangle( int numTriangle );
	void markBackfacing(const Vector3 &camPos);
	
	int LineSelect( const Vector3 &p1, const Vector3 &p2 );
	int FrustumSelect( Vector3 normals[4], Vector3 points[8] );

	enum SelectionModes{ SELECT_ADD, SELECT_SUB };
	enum TriFlags{ TF_SELECTED = 1, TF_BACKFACING = (1<<1) };	
private:
	
	// Member Data		
	SceneNode*		mSceneNode;					// Scene node of this mesh
	Entity*			mEntity;					// Entity of this mesh
	size_t			mVertexCount, mIndexCount;	// Vertex count and index count
	size_t			mTriCount;					// Triangle count
	Vector3*		mVertices;					//	All vertices
	unsigned long*	mIndices;					//	All indices of vertices
	Vector3*		mNormals;					//	Face normals
	int*			mTriFlags;					//	Selection flag for triangle
	int				mSelectionMode;				//	Selection mode
};

#endif