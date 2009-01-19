#include	"rayTriangle.h"



//����ray�M�T����(�T���Iv0,v1,v2)�O�_�����.	p.s.�����T���Ϊ������Τϭ�
bool rayTriangleIntersect( const Ray &ray, const Vector3 &v0, const Vector3 & v1, const Vector3 & v2, float *tuv )
{
	float t = 0.0, u = 0.0, v = 0.0;
	Vector3 e1 = v1 - v0;
	Vector3 e2 = v2 - v0;
	Vector3 p = crossProduct( ray.dir, e2 );
	float a = dotProduct(e1 , p);
	if( a > -EPSILON && a < EPSILON ){
		return false;
	}
	float f = 1.0f / a;
	Vector3 s = ray.start - v0;
	u = f* dotProduct(s,p);
	if( u < 0.0 || u > 1.0 ){
		return false;
	}
	Vector3 q = crossProduct( s, e1 );
	v = f* dotProduct( ray.dir , q );
	if( v < 0.0 || ( u+v > 1.0) ){
		return false;
	}
	t = f* dotProduct( e2,q );
	tuv[0] = t;
	tuv[1] = u;
	tuv[2] = v;
	return true;
}

//����ray�M�T����--���I(v0,v1,v2),�V�qe1=v1-v0,e2=v2-v0--�O�_�����.	p.s.�����T���Ϊ������Τϭ�
bool rayTriangleIntersect_v1( const Ray &ray, const Vector3 &v0, const Vector3 & e1, const Vector3 & e2, Vector3 *point, float *dist )
{
	float t = 0.0, u = 0.0, v = 0.0;
	//Vec3 e1 = v1 - v0;
	//Vec3 e2 = v2 - v0;
	Vector3 p = crossProduct( ray.dir, e2 );
	float a = dotProduct(e1 , p);
	if( a > -EPSILON && a < EPSILON ){
		return false;
	}
	float f = 1.0f / a;
	Vector3 s = ray.start - v0;
	u = f* dotProduct(s,p);
	if( u < 0.0 || u > 1.0 ){
		return false;
	}
	Vector3 q = crossProduct( s, e1 );
	v = f* dotProduct( ray.dir , q );
	if( v < 0.0 || ( u+v > 1.0) ){
		return false;
	}
	t = f* dotProduct( e2,q );
	*point = v0 + u * e1 + v * e2;
	*dist = t;
	
	return true;
}
//����v2
bool rayTriangleIntersect_v2( const Ray &ray, const Vector3 &v0, const Vector3 & e1, const Vector3 & e2, Vector3 *
point, Vector3 *tuv )
{
	float t = 0.0, u = 0.0, v = 0.0;
	Vector3 p = crossProduct( ray.dir, e2 );
	float a = dotProduct(e1 , p);
	if( a > -EPSILON && a < EPSILON ){
		return false;
	}
	float f = 1.0f / a;
	Vector3 s = ray.start - v0;
	u = f* dotProduct(s,p);
	if( u < 0.0 || u > 1.0 ){
		return false;
	}
	Vector3 q = crossProduct( s, e1 );
	v = f* dotProduct( ray.dir , q );
	if( v < 0.0 || ( u+v > 1.0) ){
		return false;
	}
	t = f* dotProduct( e2,q );

	*point = v0 + u * e1 + v * e2;
	(*tuv)[0] = t;
	(*tuv)[1] = u;
	(*tuv)[2] = v;
	
	return true;
}

bool ray_triangle( Face * facePtr, const Ray &ray, Vector3 *pointPtr, float *dist )
{
	Vector3 e1 = *(facePtr->vPtr[1]) - *(facePtr->vPtr[0]);
	Vector3 e2 = *(facePtr->vPtr[2]) - *(facePtr->vPtr[0]);
	return rayTriangleIntersect_v1( ray, *(facePtr->vPtr[0]), e1, e2, pointPtr, dist );
}

bool ray_triangle_v2( Face * facePtr, const Ray &ray, Vector3 *pointPtr, Vector3 *tuv )
{
	Vector3 e1 = *(facePtr->vPtr[1]) - *(facePtr->vPtr[0]);
	Vector3 e2 = *(facePtr->vPtr[2]) - *(facePtr->vPtr[0]);
	return rayTriangleIntersect_v2( ray, *(facePtr->vPtr[0]), e1, e2, pointPtr, tuv );
}

#if 0
//��Xray�M�̪񪺥�����T����
//�^�ǳ̪��I(Vec3 )�A�I�b���@�ӤT���ΤW�z�L�ѼƦ^��
//�z�L�ѼƦ^�Ǥ@��Face����	Null ���ܪ�ܥ�����T����
Vec3 getNearestIntersection_1( const Ray &ray, Face **iFacePtrPtr )
{
	Vec3 point;		//nearest intersection point
	Vec3 tempPoint;
	float t;
	float distance = FLT_MAX;
	*iFacePtrPtr = NULL;

	vector<Face>::iterator it;
	for( it = object->myFaceList.begin(); it != object->myFaceList.end(); ++it ){
		//if ray-triangle intersects
		if( ray_triangle( &(*it), ray, &tempPoint, &t )){
			if( t < distance ){
				distance = t;
				point = tempPoint;
				*iFacePtrPtr = &(*it);
//#define DEBUG_GNI
#ifdef DEBUG_GNI
				(*iFacePtrPtr)->printInfo();
				printf("dist:%g\n", distance);
#endif
			}
		}

	}
	return point;
}
#endif

//��Xray�M�̪񪺥�����T����
//�^��:�̪��I�b���@��Face(�T����)�W,�o��Face(�T����)������
//�z�L�ѼƦ^�ǥ�����I����m,�M������normal


Face* getNearestIntersection( const Ray &ray, std::vector<Face> &myFaceList, 
								Vector3 *pointPtr, Vector3 *normalPtr )
{
	Face *iFacePtr = NULL;
	Vector3 point;		//nearest intersection point
	Vector3 tempPoint;
	float tuvTemp[3];	//temp	�����I��tuv���Ȧs�ܼ�
	
	float distance = FLT_MAX;
	float uv[2];

	std::vector<Face>::iterator it;
	for( it = myFaceList.begin(); it != myFaceList.end(); ++it ){
		//if ray-triangle intersects
		if( rayTriangleIntersect( ray, *(it->vPtr[0]), *(it->vPtr[1]), *(it->vPtr[2]), tuvTemp ) ){
		
			if( tuvTemp[0] < distance && tuvTemp[0] > EPSILON ){
				distance = tuvTemp[0];
				uv[0] = tuvTemp[1];
				uv[1] = tuvTemp[2];
				iFacePtr = &(*it);
			}
		}
	}
	// do hit a triangle
	//����normal: n = (1-u-v)n0 + u* n1 + v* n2
	//����point: p = (1-u-v)p0 + u* p1 + v* p2
	if( iFacePtr != NULL ){
		*pointPtr =
			*(iFacePtr->vPtr[0]) *(1.0f-uv[0]-uv[1]) 
			+ *(iFacePtr->vPtr[1]) *uv[0] + *(iFacePtr->vPtr[2]) *uv[1];	

		*normalPtr =
			*(iFacePtr->nPtr[0]) *(1.0f-uv[0]-uv[1]) 
			+ *(iFacePtr->nPtr[1]) *uv[0] + *(iFacePtr->nPtr[2]) *uv[1];
	}
	return iFacePtr;
}


Face* getNearestIntersection_v1( const Ray &ray, std::vector<Face> &myFaceList, Vector3 *pointPtr )
{
	Face *iFacePtr = NULL;
	Vector3 point;		//nearest intersection point
	Vector3 tempPoint;
	float distance = FLT_MAX;
	float t;

	std::vector<Face>::iterator it;
	for( it = myFaceList.begin(); it != myFaceList.end(); ++it ){
		//if ray-triangle intersects
		if( ray_triangle( &(*it), ray, &tempPoint, &t )){
			if( t < distance && t > EPSILON ){
				distance = t;
				point = tempPoint;
				iFacePtr = &(*it);
//#define DEBUG_GNI
#ifdef DEBUG_GNI
				iFacePtr->printInfo();
				printf("dist:%g\n", distance);
#endif
			}
		}

	}
	*pointPtr = point;
	return iFacePtr;
}

Face* getNearestIntersection_v2( const Ray &ray, std::vector<Face> &myFaceList, 
								Vector3 *pointPtr, float *uv )
{
	Face *iFacePtr = NULL;
	Vector3 point;		//nearest intersection point
	Vector3 tempPoint;
	Vector3 tuvTemp;	//�Ȧs�ܼ�
	
	float distance = FLT_MAX;
	//float uv[2];

	std::vector<Face>::iterator it;
	for( it = myFaceList.begin(); it != myFaceList.end(); ++it ){
		//if ray-triangle intersects
		if( ray_triangle_v2( &(*it), ray, &tempPoint, &tuvTemp )){
			if( tuvTemp[0] < distance && tuvTemp[0] > EPSILON ){
				distance = tuvTemp[0];
				uv[0] = tuvTemp[1];
				uv[1] = tuvTemp[2];
				point = tempPoint;
				iFacePtr = &(*it);
			}
		}

	}
	*pointPtr = point;
	// do hit a triangle
	//if( iFacePtr != NULL ){//����normal: n = (1-u-v)n0 + u* n1 + v* n2
	//	*normalPtr =
	//	*(iFacePtr->nPtr[0]) *(1.0f-uv[0]-uv[1]) 
	//	+ *(iFacePtr->nPtr[1]) *uv[0] + *(iFacePtr->nPtr[2]) *uv[1];
	//}
	return iFacePtr;
}

Face* getNearestIntersection_v4( const Ray &ray, std::vector< mesh* > &meshList, 
								Vector3 *pointPtr, Vector3 *normalPtr, mesh **meshPtr )
{
	Face *iFacePtr = NULL;
	Vector3 point;		//nearest intersection point
	Vector3 tempPoint;
	float tuvTemp[3];	//temp	�����I��tuv���Ȧs�ܼ�
	
	float distance = FLT_MAX;
	float uv[2];

	//�ˬd�Ҧ���mesh object��face/�T����
	mesh *hitMesh = NULL;// ������T���άOmeshList�����@�� mesh*

	for( std::vector < mesh* >::iterator mPtr = meshList.begin(); 
		mPtr != meshList.end(); ++mPtr ){

		for( std::vector<Face>::iterator it = (*mPtr)->myFaceList.begin(); 
			it != (*mPtr)->myFaceList.end(); ++it ){
			//if ray-triangle intersects
			if( rayTriangleIntersect( ray, *(it->vPtr[0]), *(it->vPtr[1]), *(it->vPtr[2]), tuvTemp ) ){

				if( tuvTemp[0] < distance && tuvTemp[0] > EPSILON ){
					distance = tuvTemp[0];
					uv[0] = tuvTemp[1];
					uv[1] = tuvTemp[2];
					iFacePtr = &(*it);
					hitMesh = *mPtr;
				}
			}
		}
	}
	// do hit a triangle
	//����normal: n = (1-u-v)n0 + u* n1 + v* n2
	//����point: p = (1-u-v)p0 + u* p1 + v* p2
	if( iFacePtr != NULL ){
		*pointPtr = (*(iFacePtr->vPtr[0])) *(1.0f-uv[0]-uv[1]) 
			+ (*(iFacePtr->vPtr[1])) *uv[0] + (*(iFacePtr->vPtr[2])) *uv[1];	

		*normalPtr = (*(iFacePtr->nPtr[0])) *(1.0f-uv[0]-uv[1]) 
			+ (*(iFacePtr->nPtr[1]) )*uv[0] + (*(iFacePtr->nPtr[2])) *uv[1];
		//normalPtr->normalize();

		*meshPtr = hitMesh;
	}
	return iFacePtr;
}