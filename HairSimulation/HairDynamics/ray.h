#ifndef RAY_H
#define RAY_H

#include	"Vector3.h"
/********************************************************
*						���u						    *
*********************************************************/

class Ray{
public:
	Vector3 start;		//�_�I
	Vector3 dir;		//��V
	void printInfo();// �L�X������T
};


/********************************************************
*														*
*			image plane	���u���;�						*
*														*
*********************************************************/
class RayGenerator{
public:
	float w, h;						// screnn w x h, total pixels = w * h
	float W_2, H_2;					// W/2, H/2
	float right, top, near_scalar;	// image plane	near, right, top
	float blockSizeW, blockSizeH;	// image plane's block size
	Vector3 *eyeDirPtr, *eyePosPtr, *upPtr;	// eye's info
	Vector3 Right, Top, Near;			// image plane info
	// constructor
	RayGenerator( float w, float h, float right, float top, float near_scalar );//near�Qwindef.h�Ψ��FXD
	// set image plane's info	
	void setup( float w, float h, float right, float top, float near_scalar );
	// set eye's info
	void setEyeInfo( Vector3 *eyeDirPtr, Vector3 *eyePosPtr, Vector3 *upPtr );
	// ����image plane�W,��(i,j)pixel�����u
	Ray generate( float i, float j );
};

// ���եΪ����
Ray generateRay( float i, float j, float winW, float winH, float right, float top, const Vector3 & eyePos, const Vector3 &Right, const Vector3 &Top, const Vector3 &Near );



#endif