#include	<iostream>
#include	"ray.h"

void Ray::printInfo()
{
	printf("origin(%g,%g,%g)  dir(%g,%g,%g)\n",start.x, start.y, start.z, dir.x, dir.y, dir.z );
}

RayGenerator::RayGenerator( float w, float h, float right, float top, float near_scalar )
:w(w), h(h), right(right), top(top), near_scalar(near_scalar)
{
	W_2 = w / 2;
	H_2 = h / 2;
	blockSizeW = right * 2 / w;
	blockSizeH = top * 2 / h;
}

void RayGenerator::setup( float w, float h, float right, float top, float near_scalar )
{
	this->w = w;
	this->h = h;
	this->right = right;
	this->top = top;
	this->near_scalar = near_scalar;
	W_2 = w / 2;
	H_2 = h / 2;
	blockSizeW = right * 2 / w;
	blockSizeH = top * 2 / h;
}

void RayGenerator::setEyeInfo( Vector3 *eyeDirPtr, Vector3 *eyePosPtr, Vector3 *upPtr )
{
	this->eyeDirPtr = eyeDirPtr;
	this->eyePosPtr = eyePosPtr;
	this->upPtr = upPtr;
	Right = normalize( crossProduct(*eyeDirPtr, *upPtr));
	Top = normalize( crossProduct(Right, *eyeDirPtr));
	Near = near_scalar * normalize( *eyeDirPtr );
}
Ray RayGenerator::generate( float i, float j )
{
	Ray ray;
	//ray.dir = normalize( Near + (i-(W-1)/2)/(W/2)*right*Right + (j-(H-1)/2)/(H/2)*top*Top );
	Vector3 dir = Near + (i-W_2+0.5f)*blockSizeW *Right + (j-H_2+0.5f)*blockSizeH *Top;
	ray.dir = normalize(dir);
	ray.start = *eyePosPtr + dir;
	return ray;
}

Ray generateRay( float i, float j, float winW, float winH, float right, float top, const Vector3 & eyePos, const Vector3 &Right, const Vector3 &Top, const Vector3 &Near )
{
	Ray ray;
	//ray.dir = (i-(W-1)/2)/(W/2) *right *Right;
	float w_2 = (float)winW / 2;
	float h_2 = (float)winH / 2;
	
	Vector3 dir = Near + ((i - (winW-1)/2)/(winW/2)*right)*Right + ((j-(winH-1)/2)/(winH/2)*top)*Top;
	ray.dir = normalize( dir );
	ray.start = eyePos + dir;
	return ray;
}

