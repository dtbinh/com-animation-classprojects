#include	"Vector3.h"
#include	<iostream>
#include	<cmath>
#include	<cassert>

void AppVector3::normalize()
{
	float len2 = x*x + y*y + z*z;
	if( len2 != 0.0 ){
		len2 = sqrt(len2);
		x /= len2;
		y /= len2;
		z /= len2;
	}
}

float AppVector3::length() const
{
	float len2 = x*x + y*y + z*z;
	if( len2 != 0.0 ){
		return sqrt(len2);
	}
	else{
		std::cerr << "length is Zero" << std::endl;
		return len2;
	}
}

float AppVector3::squaredLength () const
{
    return x * x + y * y + z * z;
}

float AppVector3::squaredDistance(const AppVector3& rhs) const
{
    return (*this - rhs).squaredLength();
}

// operator overload
AppVector3 operator-( AppVector3 &a)
{
	AppVector3 v( -a.x, -a.y, -a.z );
	return v;
}
AppVector3 operator-( const AppVector3 &a, const AppVector3 &b )
{
	AppVector3 v( a.x - b.x, a.y - b.y, a.z - b.z );
	return v;
}
AppVector3 operator+( const AppVector3 & a, const AppVector3 & b)
{
	AppVector3 v( a.x + b.x, a.y + b.y, a.z + b.z );
	return v;
}
// dot product ¦³¼g¹ï
float operator*( const AppVector3 & a, const AppVector3 & b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}
// w = s * v   s: scalar	w,v: vector
AppVector3 operator*( float k, const AppVector3 & a)
{
	AppVector3 v( k * a.x, k * a.y, k * a.z );
	return v;
}
AppVector3 operator*( const AppVector3 & a, float k) 		// v * s	s:scalar
{
	AppVector3 v( a.x *k, a.y *k, a.z *k);
	return v;
}
// w = v / s   s: scalar   w,v: vector
AppVector3 operator/( const AppVector3 & a, float k )
{
	AppVector3 v( a.x /k, a.y /k, a.z /k);
	return v;
}
// v -= w
void operator-=( AppVector3 &v, const AppVector3 &w){				
	v.x -= w.x;
	v.y -= w.y;
	v.z -= w.z;
}
// v += w
void operator+=( AppVector3 &v, const AppVector3 &w){				
	v.x += w.x;
	v.y += w.y;
	v.z += w.z;
}
// v = s*v
void operator*=( AppVector3 &v, const float& s){				
	v.x *= s;
	v.y *= s;
	v.z *= s;
}
// v = v / s
void operator/=( AppVector3 &v, const float &s){
	v.x /= s;
	v.y /= s;
	v.z /= s;
}

AppVector3 add( AppVector3 & a, AppVector3 & b)
{
	AppVector3 v( a.x + b.x, a.y + b.y, a.z + b.z );
	return v;
}
AppVector3 subtract( AppVector3 & a, AppVector3 & b)
{	
	AppVector3 v( a.x - b.x, a.y - b.y, a.z - b.z );
	return v;
}
float dotProduct( const AppVector3 & a, const AppVector3 & b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}
AppVector3 scale( float k, AppVector3 & a)
{
	AppVector3 v( k * a.x, k * a.y, k * a.z );
	return v;
}
AppVector3 negate( AppVector3 & a)
{
	AppVector3 v( -a.x, -a.y, -a.z );
	return v;
}
AppVector3 crossProduct(const AppVector3 & a, const AppVector3 & b)
{
//Vec3 v;
//v[0] = a[1] * b[2] - a[2] * b[1];
//v[1] = a[2] * b[0] - a[0] * b[2];
//v[2] = a[0] * b[1] - a[1] * b[0];

	AppVector3 v( a[1] * b[2] - a[2] * b[1],
			a[2] * b[0] - a[0] * b[2],
			a[0] * b[1] - a[1] * b[0] );
	return v;
}
AppVector3 normalize( AppVector3 & a)
{
	AppVector3 v = a;

	float len2 = a.x*a.x + a.y*a.y + a.z*a.z;
	if( len2 != 0.0 ){
		len2 = sqrt(len2);
		v.x /= len2;
		v.y /= len2;
		v.z /= len2;
	}
	return v;
}
float length( AppVector3 & a)
{
	float len2 = a.x*a.x + a.y*a.y + a.z*a.z;
	if( len2 != 0.0 ){
		return sqrt(len2);
	}
	else
		return len2;
}




/* ÂÂª©¥» */
void VecAdd( AppVector3 &a, AppVector3 &b, AppVector3 &result )
{
	for( int i = 0; i < 3; i++ ){
		result[i] = a[i] + b[i];
	}
}
void VecSub( AppVector3 &a, AppVector3 &b, AppVector3 &result )
{
	for( int i = 0; i < 3; i++ ){
		result[i] = a[i] - b[i];
	}	
}
void VecScale( AppVector3 &a, float s, AppVector3 &result )
{
	for( int i = 0; i < 3; i++ )
		result[i] = a[i] * s;
}
float VecDot( AppVector3 &a, AppVector3 &b )
{
	float r = 0.0;
	for(int i = 0; i<3; i++){
		r += a[i] * b[i];
	}
	return r;
}
void VecCross( AppVector3 &a, AppVector3 &b, AppVector3 &result )
{
	result[0] = a[1] * b[2] - a[2] * b[1];
	result[1] = a[2] * b[0] - a[0] * b[2];
	result[2] = a[0] * b[1] - a[1] * b[0];
}
float VecLength( AppVector3 &a )
{
	float len2 = a[0]*a[0] + a[1]*a[1] + a[2]*a[2];
	if( len2 != 0.0 ){
		len2 = sqrt( len2 );
	}
	return len2;
}
void NormalizeVec( AppVector3 &a )
{
	float len2 = a[0]*a[0] + a[1]*a[1] + a[2]*a[2];
	if( len2 != 0.0 ){
		float len = sqrt( len2 );
		if( len != 0.0 ){
			a[0] /= len;
			a[1] /= len;
			a[2] /= len;
		}
	}

}
void NegateVec( AppVector3 &a )
{
	a[0] = -a[0];
	a[1] = -a[1];
	a[2] = -a[2];
}

void VecAdd( Vec4 &a, Vec4 &b, Vec4 &result )
{
	for( int i = 0; i < 4; i++ ){
		result[i] = a[i] + b[i];
	}
}
void VecSub( Vec4 &a, Vec4 &b, Vec4 &result )
{
	for( int i = 0; i < 4; i++ ){
		result[i] = a[i] - b[i];
	}	
}
void VecScale( Vec4 &a, float s, Vec4 &result )
{
	for( int i = 0; i < 4; i++ )
		result[i] = a[i] * s;
}

void NormalizeVec( Vec4 &a )
{
	float len2 = a[0]*a[0] + a[1]*a[1] + a[2]*a[2];
	if( len2 != 0.0 ){
		float len = sqrt( len2 );
		if( len != 0.0 ){
			a[0] /= len;
			a[1] /= len;
			a[2] /= len;
		}
	}

}