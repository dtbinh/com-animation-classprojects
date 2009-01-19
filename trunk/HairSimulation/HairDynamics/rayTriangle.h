#ifndef RAY_TRIANGLE_H
#define RAY_TRIANGLE_H

#include	"ray.h"
#include	"mesh.h"

#include	<cfloat>
#define EPSILON 1e-7
#define FLT_EPSILON_MIN 1E-4

//測試ray和三角形(三個點v0,v1,v2)是否有交錯.	p.s.不分三角形的正面或反面
bool rayTriangleIntersect( const Ray &ray, const Vector3 &v0, const Vector3 & v1, const Vector3 & v2, float *tuv );

//測試ray和三角形--頂點(v0,v1,v2),向量e1=v1-v0,e2=v2-v0--是否有交錯.	p.s.不分三角形的正面或反面
bool rayTriangleIntersect_v1( const Ray &ray, const Vector3 &v0, const Vector3 & e1, const Vector3 & e2, Vector3 *point, float *dist );

bool rayTriangleIntersect_v2( const Ray &ray, const Vector3 &v0, const Vector3 & e1, const Vector3 & e2, Vector3 *
point, Vector3 *tuv );


bool ray_triangle( Face * facePtr, const Ray &ray, Vector3 *pointPtr, float *dist );

bool ray_triangle_v2( Face * facePtr, const Ray &ray, Vector3 *pointPtr, Vector3 *tuv );


//找出ray和最近的交錯的三角形
//回傳最近的點(Vec3 )，點在哪一個三角形上透過參數回傳
//透過參數回傳一個Face指標	Null 的話表示交錯的三角形
//Vec3 getNearestIntersection( const Ray &ray, Face **iFacePtrPtr );


//找出ray和最近的交錯的三角形
//回傳:最近的點在哪一個Face(三角形)上,這個Face(三角形)的指標
//透過參數回傳交錯的點的位置
Face* getNearestIntersection( const Ray &ray, std::vector<Face> &myFaceList, Vector3 *pointPtr, Vector3 *normalPtr );

Face* getNearestIntersection_v1( const Ray &ray, std::vector<Face> &myFaceList, Vector3 *pointPtr );

Face* getNearestIntersection_v2( const Ray &ray, std::vector<Face> &myFaceList, Vector3 *pointPtr, float *uv );

//檢查所有mesh object, all faces
Face* getNearestIntersection_v4( const Ray &ray, std::vector< mesh* > &meshList, 
								Vector3 *pointPtr, Vector3 *normalPtr, mesh **meshPtr );
							
#endif