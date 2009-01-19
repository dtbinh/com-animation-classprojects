
#include	<cassert>
#include	<vector>
//#include	<list>
#include	"aabb.h"

//~~~~~~~~~ AabbTree ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// prototype
bool sphereTri3D_v2( const Vector3 & A, const Vector3 & B, const Vector3 & C,
		const Vector3 & N, const Vector3 & P, float R2 );

bool AabbTree::testBallTri( const Vector3 & C, float R2 ) const
{
	return ballTriTester( root, C, R2 );
}
bool AabbTree::ballTriTester( TreeNode *tptr, const Vector3 &C, float R2 ) const
{
	if( tptr->box.BallBoxIntersect( C, R2 ) ){
		if( tptr->left == NULL ){	// leaf node
			return sphereTri3D_v2( *tptr->facePtr->vPtr[0], *tptr->facePtr->vPtr[1], *tptr->facePtr->vPtr[2],
				*tptr->facePtr->fnPtr, C, R2 );
		}else{
			return ( ballTriTester( tptr->left, C, R2 ) ||
					 ballTriTester( tptr->right, C, R2 ) );
		}
	}
	return false;
}
// return which Tri
Face* AabbTree::testBallTri_v2( const Vector3 & C, float R2 ) const
{
	return ballTriTester_v2( root, C, R2 );
}
Face* AabbTree::ballTriTester_v2( TreeNode *tptr, const Vector3 &C, float R2 ) const
{
	if( tptr->box.BallBoxIntersect( C, R2 ) ){
		if( tptr->left == NULL ){	// leaf node
			if( sphereTri3D_v2( *tptr->facePtr->vPtr[0], *tptr->facePtr->vPtr[1], *tptr->facePtr->vPtr[2], *tptr->facePtr->fnPtr, C, R2 ) ){
				return tptr->facePtr;
			}else{
				return NULL;
			}
		}else{
			Face *tri = NULL;
			if( ( tri = ballTriTester_v2( tptr->left, C, R2 ) ) != NULL ){
				return tri;
			}else{
				return ballTriTester_v2( tptr->right, C, R2 );
			}
		}
	}
	return NULL;
}

void AabbTree::display() const
{	
	// ���� �� �� �� �G�� ���� �ź� ���� �L�� �`�� �� ���� 
	Vector3 colorTable[12] = { Vector3(1, 0.6, 0.8), Vector3(1, 0, 0), Vector3(1, 0.6, 0), Vector3(1, 1, 0.6),
							Vector3(0.6, 1, 0.2), Vector3(0, 0.5, 0), Vector3(0, 1, 0.6),
							Vector3(0, 0.8, 1), Vector3(0.6, 0.8, 1), Vector3(0.2, 0.2, 1),
							Vector3(0.6, 0.2, 1), Vector3(1, 0.2, 0.8) };
	int depth = 0;
	postorderVisitor( root, depth, colorTable );
}

void AabbTree::postorderVisitor( TreeNode *tptr, int depth, Vector3 *colorTable ) const
{
	if( tptr != NULL ){
		
		//visit node

		Vector3 *c = colorTable + ( depth  );
		glColor3f( c->x, c->y, c->z );
		
		GLfloat Ks[] = { 0 , 0 , 0 };
		GLfloat Ns = 160;
		glMaterialfv(GL_FRONT, GL_AMBIENT  ,  c->ptr );	//��box��ambient�C��
		glMaterialfv(GL_FRONT, GL_DIFFUSE  ,  c->ptr);	//��box��diffuse�C��
		glMaterialfv(GL_FRONT, GL_SPECULAR ,  Ks);		//��box��specular�C��
		glMaterialf(GL_FRONT, GL_SHININESS,   Ns);		//��box��shininess�j��		

		tptr->box.display();

		depth++;
		depth %= 12;
		postorderVisitor( tptr->left, depth, colorTable );
		postorderVisitor( tptr->right, depth, colorTable );
	}
}


void AabbTree::preorder()
{
	preorderVisitor(root);
}
void AabbTree::preorderVisitor( TreeNode *tptr )
{
	if( tptr != NULL ){
		//visit node
		if( tptr->left == NULL ){
			printf("tree leaf: ");
			//for(int j = 0; j < 3; j++ ){
			//	Vec3 v = m_meshData->vList[ (tptr->facePtr)->v[j].v ];
			//	printf( "(%g,%g,%g)", v.x, v.y, v.z );
			//}
			printf("\n");
		}
		preorderVisitor( tptr->left );
		preorderVisitor( tptr->right );
	}
}

void AabbTree::buildTree( std::vector<Face> &faceList, int faceTotal )
{
	//setup Face* m_facePtrArray
	
	m_faceTotal = faceTotal;
	m_facePtrArray = new Face*[faceTotal];

	assert( m_facePtrArray != 0 );
	int i = 0;
	for( std::vector<Face>::iterator fPtr = faceList.begin();
		fPtr != faceList.end(); ++fPtr )
	{
		m_facePtrArray[i] = &(*fPtr);
		++i;
	}
	
	//�ظ`�I
	root = NULL;
	insertTreeNode( &root, m_facePtrArray, 0, m_faceTotal-1 );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ���j�إX�@�� Aabb �� tree => BV tree
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AabbTree::insertTreeNode( TreeNode ** nodePtr, Face **fArray, int start, int end )
{
	// if subtree is empty, create new TreeNode containing value
	assert( *nodePtr == 0 );

	Vector3 max, min, center;
	// ��@�s�T���Ϊ���T
	calcTriangleListInfo( fArray, start, end, max, min, center );

	*nodePtr = new TreeNode();
	(*nodePtr)->facePtr = NULL;
	(*nodePtr)->left = (*nodePtr)->right = NULL;
	BoundBox box;
	box.center = (max+min)/2;
	box.dim=(max-min)/2;
	box.bounds[0] = min;
	box.bounds[1] = max;
	(*nodePtr)->box = box;
	assert( max[0] >= min[0] );
	assert( max[1] >= min[1] );
	assert( max[2] >= min[2] );

	// recurssion stop criteria: num of triangle == 1
	if( (end-start+1) > 1 ){

		int longestAxis = box.longestAxis();

		//�N�T���� Array �� partition, �H�T���� centroid �b���b�䰵����
		// i ���䪺�� <= k
		// j �h�� <= k ���ȥ洫
		int i = start - 1;
		for( int j = start; j <= end; j++ ){
			Vector3 c = calcTriangleCenter( fArray[j] );
			if( c[longestAxis] <= box.center[longestAxis] ){
				i++;
				//swap A[i],A[j]
				if( i != j ){
					Face* temp = fArray[i];
					fArray[i] = fArray[j];
					fArray[j] = temp;
				}
			}
		}
		//�B�z�p�G���b�� / �k�b��S������@�ӤT���Ϊ����p
		//�۰ʲ��@�浹�S�������b��
		if( i == start-1 ){
			i = start;
		}else if( i == end ){
			i = end-1;
		}
		//�Ϥ��� [start,i], [i+1, end]

		//�� recursion
		insertTreeNode( &((*nodePtr)->left), fArray, start, i );
		insertTreeNode( &((*nodePtr)->right), fArray, i+1, end );
	}
	else{ // stop recurssion
		(*nodePtr)->facePtr = fArray[start];
	}
	//�ا� TreeNode	
}

void AabbTree::update()
{
	postOrderUpdate( root );
}

void AabbTree::postOrderUpdate( TreeNode *tPtr )
{
	// leaf node
	// �o�� tree �� non-leaf node �@�w�� left,right child
	// �ˬd left or right �N���O���O leaf node
	if( tPtr->left == 0 ){

		// update bounding volume
		Vector3 max, min;
		max = min = *(tPtr->facePtr->vPtr[0]);

		for( int i = 1; i < 3; i++ ){
			for( int j = 0; j < 3; ++j ){
				
				if( (*(tPtr->facePtr->vPtr[i]))[j] > max[j] ){
					max[j] = (*(tPtr->facePtr->vPtr[i]))[j];
				}else if( (*(tPtr->facePtr->vPtr[i]))[j] < min[j] ){
					min[j] = (*(tPtr->facePtr->vPtr[i]))[j];
				}
			}
		}
		tPtr->box.center = (max+min)/2;
		tPtr->box.dim=(max-min)/2;
		return;

	}else{
		postOrderUpdate( tPtr->left );
		assert( tPtr->right != 0 );
		postOrderUpdate( tPtr->right );

		//merge children's bounding volume
		Vector3 min, max;
		Vector3 minL, maxL;	//left box's min,max
		Vector3 minR, maxR;	//right box's min,max

		BoundBox *b = &(tPtr->left->box);
		minL = Vector3( b->center[0] -b->dim[0], b->center[1] -b->dim[1], b->center[2] -b->dim[2] );
		maxL = Vector3( b->center[0] +b->dim[0], b->center[1] +b->dim[1], b->center[2] +b->dim[2] );

		b = &(tPtr->right->box);
		minR = Vector3( b->center[0] -b->dim[0], b->center[1] -b->dim[1], b->center[2] -b->dim[2] );
		maxR = Vector3( b->center[0] +b->dim[0], b->center[1] +b->dim[1], b->center[2] +b->dim[2] );
		
		for( int j = 0; j < 3; ++j ){
			max[j] = (maxL[j] > maxR[j] ? maxL[j] : maxR[j] );
			min[j] = (minL[j] < minR[j] ? minL[j] : minR[j] );
		}
		
		tPtr->box.center = (max+min)/2;
		tPtr->box.dim=(max-min)/2;
		tPtr->box.bounds[0] = min;
		tPtr->box.bounds[1] = max;
		assert( max[0] >= min[0] );
		assert( max[1] >= min[1] );
		assert( max[2] >= min[2] );
	}
}




/**********************************************************************************************
* ��@�C�T���Ϊ� x,y,z �̤j�̤p��
* �T���θ�Ʀs�b facePtrArray, (start,end) = Array �_�l/���� index
* �^�ǩ�b max, min, center
**********************************************************************************************/
void AabbTree::calcTriangleListInfo( Face** facePtrArray, int start, int end, Vector3 &max, Vector3 &min, Vector3 &center )
{
	//list< Face* >::iterator iter = faceList.begin();
	center = Vector3(0, 0, 0);
	// foreach face, foreach vertex, for x,y,z

	//max = min = m_meshData->vList[ (*iter)->v[0].v ];
	//for( iter = faceList.begin(); iter != faceList.end(); iter++ ){

	//	for( int j = 1; j < 3; j++ ){
	//		for( int i = 0; i < 3; i++ ){
	//			if( max[i] < m_meshData->vList[ (*iter)->v[j].v ][i] ){
	//				max[i] = m_meshData->vList[ (*iter)->v[j].v ][i];
	//			}else if( min[i] > m_meshData->vList[ (*iter)->v[j].v ][i] ){
	//				min[i]  = m_meshData->vList[ (*iter)->v[j].v ][i];
	//			}
	//		}
	//	}
	//	center = center + calcTriangleCenter( *iter );
	//}
	//center = center / m_meshData->fTotal; ?? center / faceList.size()

	max = min = *facePtrArray[start]->vPtr[0] ;
	// for each �T���� in array
	//		for each ���I in �T����
	//			for x,y,z in ���I
	//				�M max, min ��x,y,z �����
	for(int i = start; i <= end; i++ ){
		for( int j = 0; j < 3; j++ ){
			for( int k = 0; k < 3; k++ ){
				if( max[k] < (*(facePtrArray[i]->vPtr[j])) [k] ){
					max[k] = (*(facePtrArray[i]->vPtr[j])) [k];
				}else if( min[k] > (*(facePtrArray[i]->vPtr[j])) [k] ){
					min[k] = (*(facePtrArray[i]->vPtr[j])) [k];
				}
			}
			center += (*(facePtrArray[i]->vPtr[j])) ;
		}
	}
	center /= ((end - start + 1) * 3);

#ifdef DEBUG
	Vec3 v = max;
	printf( "%g %g %g\n", v.x, v.y, v.z );
	v = min;
	printf( "%g %g %g\n", v.x, v.y, v.z );
	v = center;
	printf( "%g %g %g\n", v.x, v.y, v.z );
#endif
}


// calculate centroid/ geometry center/ barycenter of a triangle
Vector3 AabbTree::calcTriangleCenter( Face* facePtr )
{
	Vector3 center( 0, 0, 0 );
	for( int j = 0; j < 3; j++ ){
		center = center + *(facePtr->vPtr[j]) ;
	}
	center = center / 3;

#ifdef DEBUG2
	Vec3 &v = center;
	printf( "%g %g %g\n", v.x, v.y, v.z );
#endif
	return center;
}


#if 0
void AabbTree::calculateTriangleSize( Face* facePtr, Vec3& max, Vec3& min ){
	// face����j���I��vertex
	// 	m_meshData->vList[ face.v[j].v ].ptr

	max = min = m_meshData->vList[ facePtr->v[0].v ];
	for( int j = 1; j < 3; j++ ){
		for( int i = 0; i < 3; i++ ){
			if( max[i] < m_meshData->vList[ facePtr->v[j].v ][i] ){
				max[i] = m_meshData->vList[ facePtr->v[j].v ][i];
			}else if( min[i] > m_meshData->vList[ facePtr->v[j].v ][i] ){
				min[i]  = m_meshData->vList[ facePtr->v[j].v ][i];
			}
		}
	}
}
#endif