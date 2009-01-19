#ifndef CAMERA_H
#define CAMERA_H

#include	"Vector3.h"

class Camera{
public:
	AppVector3 eyePos;	//gluLookAt( eye[3], center[3], up[3] );
	AppVector3 center;
	AppVector3 up;
	AppVector3 eyeDir;//鏡頭朝向的方向
	AppVector3 right;	//鏡頭平面右邊的方向
	AppVector3 top;	//鏡頭平面上面的方向

	Camera()
		//:eyePos( 0.0f, 23.0f, 26.0f ), center( 0.0f, 20.0f, 10.0f ), up( 0.0f, 1.0f, 0.0f )
		//:eyePos( 5.1f, 26.0f, 10.7f ), center( 16.4f, 27.0f, -1.9f ), up( 0.0f, 1.0f, 0.0f )
		//:eyePos( 4.66f, 27.0f, 11.2f ), center( 16.1f, 27.0f, -1.6f ), up( 0.0f, 1.0f, 0.0f )	// demo collision
		:eyePos( 2.943f, 27.0f, 8.72f ), center( 17.0f, 27.0f, -1.15f ), up( 0.0f, 1.0f, 0.0f )	// demo collision2
	{
		recalculate();
	}

	inline void printInfo()
	{
		printf("--------camera--------\n");
		printf("pos:");
		eyePos.printInfo();
		printf("dir");
		eyeDir.printInfo();
		printf("center");
		center.printInfo();
		printf("right");
		right.printInfo();
		printf("top");
		top.printInfo();
		
	}
	//設定相機：pos 位置,aim_pos/center 朝向的位置, up 定義垂直平面的向量
	//相機會自己算出:鏡頭/目光的方向,和鏡頭平面中,上下/左右的方向
	void setup( const AppVector3 &eyePos, const AppVector3 &center, const AppVector3 &up )
	{
		this->eyePos = eyePos;
		this->center = center;
		this->up = up;
		eyeDir = normalize( center - eyePos );
		right = normalize( crossProduct( eyeDir, up ) );
		top = normalize( crossProduct( right, eyeDir ) );
	}
	void recalculate()
	{
		eyeDir = normalize( center - eyePos );
		right = normalize( crossProduct( eyeDir, up ) );
		top = normalize( crossProduct( right, eyeDir ) );
	}
	//鏡頭往左/往右/往上/往下移
	void turnUpDown( float t )
	{
		center += (top * t);
		recalculate();
	}
	void turnRightLeft(float r)
	{
		center += (right * r);
		recalculate();
	}
	void zoomInOut( float in )
	{
		eyePos += ( eyeDir * in );
		recalculate();
	}
	//移動相機位置(視野)
	void moveUpDown( float m)
	{
		eyePos += ( m* top );
		center += ( m* top );
		//recalculate();
	}
	void moveRightLeft( float m )
	{
		eyePos += ( m* right );
		center += ( m* right);
		//recalculate();
	}
	void moveForthBack( float m )
	{
		eyePos += ( m* eyeDir );
		center += ( m* eyeDir );
		//recalculate();
	}
};

#endif