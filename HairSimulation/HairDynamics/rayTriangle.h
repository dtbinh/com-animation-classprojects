#ifndef RAY_TRIANGLE_H
#define RAY_TRIANGLE_H

#include	"ray.h"
#include	"mesh.h"

#include	<cfloat>
#define EPSILON 1e-7
#define FLT_EPSILON_MIN 1E-4

//����ray�M�T����(�T���Iv0,v1,v2)�O�_�����.	p.s.�����T���Ϊ������Τϭ�
bool rayTriangleIntersect( const Ray &ray, const Vector3 &v0, const Vector3 & v1, const Vector3 & v2, float *tuv );

//����ray�M�T����--���I(v0,v1,v2),�V�qe1=v1-v0,e2=v2-v0--�O�_�����.	p.s.�����T���Ϊ������Τϭ�
bool rayTriangleIntersect_v1( const Ray &ray, const Vector3 &v0, const Vector3 & e1, const Vector3 & e2, Vector3 *point, float *dist );

bool rayTriangleIntersect_v2( const Ray &ray, const Vector3 &v0, const Vector3 & e1, const Vector3 & e2, Vector3 *
point, Vector3 *tuv );


bool ray_triangle( Face * facePtr, const Ray &ray, Vector3 *pointPtr, float *dist );

bool ray_triangle_v2( Face * facePtr, const Ray &ray, Vector3 *pointPtr, Vector3 *tuv );


//��Xray�M�̪񪺥�����T����
//�^�ǳ̪��I(Vec3 )�A�I�b���@�ӤT���ΤW�z�L�ѼƦ^��
//�z�L�ѼƦ^�Ǥ@��Face����	Null ���ܪ�ܥ�����T����
//Vec3 getNearestIntersection( const Ray &ray, Face **iFacePtrPtr );


//��Xray�M�̪񪺥�����T����
//�^��:�̪��I�b���@��Face(�T����)�W,�o��Face(�T����)������
//�z�L�ѼƦ^�ǥ�����I����m
Face* getNearestIntersection( const Ray &ray, std::vector<Face> &myFaceList, Vector3 *pointPtr, Vector3 *normalPtr );

Face* getNearestIntersection_v1( const Ray &ray, std::vector<Face> &myFaceList, Vector3 *pointPtr );

Face* getNearestIntersection_v2( const Ray &ray, std::vector<Face> &myFaceList, Vector3 *pointPtr, float *uv );

//�ˬd�Ҧ�mesh object, all faces
Face* getNearestIntersection_v4( const Ray &ray, std::vector< mesh* > &meshList, 
								Vector3 *pointPtr, Vector3 *normalPtr, mesh **meshPtr );
							
#endif