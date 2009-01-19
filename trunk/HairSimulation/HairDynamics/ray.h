#ifndef RAY_H
#define RAY_H

#include	"Vector3.h"
/********************************************************
*						光線						    *
*********************************************************/

class Ray{
public:
	Vector3 start;		//起點
	Vector3 dir;		//方向
	void printInfo();// 印出相關資訊
};


/********************************************************
*														*
*			image plane	光線產生器						*
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
	RayGenerator( float w, float h, float right, float top, float near_scalar );//near被windef.h用走了XD
	// set image plane's info	
	void setup( float w, float h, float right, float top, float near_scalar );
	// set eye's info
	void setEyeInfo( Vector3 *eyeDirPtr, Vector3 *eyePosPtr, Vector3 *upPtr );
	// 產生image plane上,第(i,j)pixel的光線
	Ray generate( float i, float j );
};

// 測試用的函數
Ray generateRay( float i, float j, float winW, float winH, float right, float top, const Vector3 & eyePos, const Vector3 &Right, const Vector3 &Top, const Vector3 &Near );



#endif