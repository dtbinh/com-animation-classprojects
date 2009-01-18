#include "Mesh.h"
#include "AppUtilities.h"
#include "intersection.h"
#include "World.h"

//----------------------------------------------------------
CMesh::CMesh(SceneNode* sceneNode, Entity* entity) 
	: mSceneNode(sceneNode), mEntity(entity)
{
	mVertexCount = 0;
	mIndexCount = 0;
	mTriCount = 0;
	mVertices = 0;
	mIndices = 0;
	mNormals = 0;
	mTriFlags = 0;
	mSelectionMode = SELECT_ADD;
	mVisualMesh = new DynamicLines(ColourValue(0.0f, .4f, .8f),
		RenderOperation::OT_LINE_LIST);
	mSelectedFaces = World::getSingleton().getSceneManager()->createManualObject("SelectedFaces");

	mMeshSceneNode = World::getSingleton().getSceneManager()->getRootSceneNode()->createChildSceneNode();
	mMeshSceneNode->attachObject(mVisualMesh);
	mMeshSceneNode->attachObject(mSelectedFaces);
}

//----------------------------------------------------------
CMesh::~CMesh()
{
	delete[] mVertices;
	delete[] mIndices;
	delete[] mNormals;
	delete[] mTriFlags;
	mMeshSceneNode->detachAllObjects();
	delete mVisualMesh;
	World::getSingletonPtr()->getSceneManager()->destroyManualObject("SelectedFaces");
}

//----------------------------------------------------------
void CMesh::generateMeshInfo(void)
{
	if ((mSceneNode == NULL) || (mEntity == NULL))
	{
		std::cout << "Error in CMesh::generateMeshInfo()" << std::endl;
		return;
	}

	Utilities::getMeshInformation(mEntity->getMesh(), mVertexCount, mVertices, mIndexCount, mIndices,
		mEntity->getParentNode()->_getDerivedPosition(), 
		mEntity->getParentNode()->_getDerivedOrientation(),
		mEntity->getParentNode()->_getDerivedScale());

	mTriCount = mIndexCount / 3;
	// Allocate space for mNormals and mTriFlags
	mNormals = new Vector3[mTriCount];
	mTriFlags = new int[mTriCount];

	//	Initialize mNormals
	memset(mTriFlags, 0, mTriCount*sizeof(int));

	// Calculate face normals
	calculateFaceNormals();

}

//----------------------------------------------------------
void CMesh::calculateFaceNormals(void)
{
	int fIndex, vIndex;
	Vector3 p0, p1, p2;

	for (fIndex = 0; fIndex < (int)mTriCount; fIndex++)
	{
		vIndex = fIndex*3;
		p0 = mVertices[mIndices[vIndex]];
		p1 = mVertices[mIndices[vIndex+1]];
		p2 = mVertices[mIndices[vIndex+2]];
		mNormals[fIndex] = (p1-p0).crossProduct(p2-p0);
		mNormals[fIndex].normalise();
	}
}

//----------------------------------------------------------
void CMesh::renderAllMesh(void)
{
	int fIndex, vIndex;
	Vector3 p1, p2, p3;
	
	// Render the wireframe
	for (fIndex = 0; fIndex < (int)mTriCount; fIndex++)
	{
		vIndex = fIndex*3;
		p1 = mVertices[mIndices[vIndex]];
		p2 = mVertices[mIndices[vIndex+1]];
		p3 = mVertices[mIndices[vIndex+2]];
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

//----------------------------------------------------------
void CMesh::renderSelectedFaces()
{
	int fIndex, vIndex;
	Vector3 p1, p2, p3;

	// Clear existed in mSelectedMesh
	mSelectedFaces->clear();

	mSelectedFaces->begin("BaseWhiteNoLighting", RenderOperation::OT_TRIANGLE_LIST);
	mSelectedFaces->colour(ColourValue::Red);
	// Render the wireframe
	for (fIndex = 0; fIndex < (int)mTriCount; fIndex++)
	{
		if ((mTriFlags[fIndex] & TF_SELECTED) &&
			 !(mTriFlags[fIndex] & TF_BACKFACING))
		{
			vIndex = fIndex*3;
			p1 = mVertices[mIndices[vIndex]];
			p2 = mVertices[mIndices[vIndex+1]];
			p3 = mVertices[mIndices[vIndex+2]];
		
			mSelectedFaces->position(p1);
			mSelectedFaces->position(p2);
			mSelectedFaces->position(p3);
		}
	}

	mSelectedFaces->end();

	mMeshSceneNode->needUpdate(true);
}
	
//----------------------------------------------------------
void CMesh::clearSelection()
{
	for (int i = 0; i < (int)mTriCount; i++)
	{
		mTriFlags[i] &= ~TF_SELECTED;
	}
}

//----------------------------------------------------------
void CMesh::setSelectionMode( int mode )
{
	mSelectionMode = mode;
}

//----------------------------------------------------------
void CMesh::selectTriangle( int triIndex )
{
	if (triIndex < 0 || triIndex >= (int)mTriCount) return;
	if (mSelectionMode == SELECT_ADD) mTriFlags[triIndex] |= TF_SELECTED;
	if (mSelectionMode == SELECT_SUB) mTriFlags[triIndex] &= ~TF_SELECTED;
}

//---------------------------------------------------------
/** Set the TF_BACKFACING flag for triangles that are backfacing from Camera's point of view */
void CMesh::markBackfacing(const Vector3 &camPos)
{
	for (int i = 0; i < (int)mTriCount; i++)
	{
		if (mNormals[i].dotProduct(camPos - mVertices[mIndices[i*3]]) < 0)
			mTriFlags[i] |= TF_BACKFACING;
		else
			mTriFlags[i] &= ~TF_BACKFACING;
	}
}
	
//---------------------------------------------------------
// Test a line against a mesh
// Select the closest front-facing triangle
int CMesh::LineSelect( const Vector3 &LP1, const Vector3 &LP2 )
{
	Vector3 HitP;
	int nbHits = 0;
	int nSelTri = -1;
	float fDistance = 1000000000.0f;
	
	for (int nTri = 0; nTri < (int)mTriCount; nTri++ )
		{
		if ( mTriFlags[ nTri ] & TF_BACKFACING ) continue; // Check only front facing triangles
		
		int nV = nTri*3;	

		bool bHit = CheckLineTri( LP2, LP1, mVertices[ mIndices[nV] ], mVertices[ mIndices[nV+1] ], mVertices[ mIndices[nV+2] ], HitP );
		if ( bHit ) {
			if ( HitP.distance( LP1 ) < fDistance ) {
				fDistance = HitP.distance( LP1 );
				nSelTri = nTri;
				}
			nbHits++;
			}
		}
		
	selectTriangle( nSelTri );
	
	return nbHits;
}

//---------------------------------------------------------
int CMesh::FrustumSelect( Vector3 Normals[4], Vector3 Points[8] )
{

	int nbHits = 0;
	Vector3 Tri[3];
	
	for (int nTri = 0; nTri < (int)mTriCount; nTri++ )
	{	
		int nV = nTri*3;
		Tri[0] = mVertices[ mIndices[ nV ] ];
		Tri[1] = mVertices[ mIndices[ nV+1 ] ];
		Tri[2] = mVertices[ mIndices[ nV+2 ] ];

		if ( TriInFrustum( Tri, Normals, Points ) )
		{
			selectTriangle( nTri );			
			nbHits++;			
		}
	}
	return nbHits;
}

//-------------------------------------------------------------------------
size_t CMesh::getVertexCount(void)
{
	return mVertexCount;
}

//-------------------------------------------------------------------------
size_t CMesh::getIndexCount(void)
{
	return mIndexCount;
}

//-------------------------------------------------------------------------
Vector3* CMesh::getVertices(void)
{
	return mVertices;
}

//-------------------------------------------------------------------------
unsigned long* CMesh::getIndices(void)
{
	return mIndices;
}

//-------------------------------------------------------------------------
int* CMesh::getTriFlags(void)
{
	return mTriFlags;
}

//-------------------------------------------------------------------------
Vector3* CMesh::getNormals(void)
{
	return mNormals;
}

//-------------------------------------------------------------------------
size_t CMesh::getTriCount()
{
	return mTriCount;
}
	
	