#ifndef AABB_H
#define AABB_H

//#include	<list>
#include	<cassert>
//#include	"vector3.h"
#include	"mat_face.h"
#include	"box.h"


// fit to mesh object
//~~~~~~~~~~~~~ TreeNode ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class AabbTree;

class TreeNode{
public:
	BoundBox box;
	Face* facePtr;
	TreeNode *left;
	TreeNode *right;
};


class AabbTree{

public:
	// Constructor
	AabbTree()
		:root(0), m_facePtrArray(0), m_faceTotal(0)
	{}

	TreeNode *root;
	void buildTree( std::vector<Face> &faceList, int faceTotal );
	void insertTreeNode( TreeNode ** nodePtr, Face** fArray, int start, int end );
	void update();
	void postOrderUpdate( TreeNode *tPtr );
	void preorder();
	void preorderVisitor( TreeNode * );
	void display() const;
	void postorderVisitor( TreeNode *tptr, int depth, Vector3 *colorTable ) const;
	// test Ball-Tri overlap
	bool testBallTri( const Vector3 & C, float R2 ) const;
	bool ballTriTester( TreeNode *tptr, const Vector3 &C, float R2 ) const;
	Face* testBallTri_v2( const Vector3 & C, float R2 ) const;
	Face* ballTriTester_v2( TreeNode *tptr, const Vector3 &C, float R2 ) const;

public:
	//mesh * m_meshData;
	//list< Face* > faceList;
	Face **m_facePtrArray;	//dynamic array: an Array of pointer to face(s)
	int m_faceTotal;
	//void setup( std::vector<Face> &faceList );
	//void setupFaceList();

	void calcTriangleListInfo( Face** facePtrArray, int start, int end, Vector3 &max, Vector3 &min, Vector3 &center );
	//void calcTriangleListInfo( std::list<Face*> &faceList, Vec3& max, Vec3 & min, Vec3 &center );
	//void calculateTriangleSize( Face* facePtr, Vec3 & max, Vec3& min );
	Vector3 calcTriangleCenter( Face* facePtr );

	//AabbTree(const AabbTree & );
};

#endif