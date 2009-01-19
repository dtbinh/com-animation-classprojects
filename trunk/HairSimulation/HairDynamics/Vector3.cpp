#include	"Vector3.h"
#include	<iostream>
#include	<cmath>
#include	<cassert>

void Vector3::normalize()
{
	float len2 = x*x + y*y + z*z;
	if( len2 != 0.0 ){
		len2 = sqrt(len2);
		x /= len2;
		y /= len2;
		z /= len2;
	}
}

float Vector3::length() const
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

float Vector3::squaredLength () const
{
    return x * x + y * y + z * z;
}

float Vector3::squaredDistance(const Vector3& rhs) const
{
    return (*this - rhs).squaredLength();
}

// operator overload
Vector3 operator-( Vector3 &a)
{
	Vector3 v( -a.x, -a.y, -a.z );
	return v;
}
Vector3 operator-( const Vector3 &a, const Vector3 &b )
{
	Vector3 v( a.x - b.x, a.y - b.y, a.z - b.z );
	return v;
}
Vector3 operator+( const Vector3 & a, const Vector3 & b)
{
	Vector3 v( a.x + b.x, a.y + b.y, a.z + b.z );
	return v;
}
// dot product ¦³¼g¹ï
float operator*( const Vector3 & a, const Vector3 & b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}
// w = s * v   s: scalar	w,v: vector
Vector3 operator*( float k, const Vector3 & a)
{
	Vector3 v( k * a.x, k * a.y, k * a.z );
	return v;
}
Vector3 operator*( const Vector3 & a, float k) 		// v * s	s:scalar
{
	Vector3 v( a.x *k, a.y *k, a.z *k);
	return v;
}
// w = v / s   s: scalar   w,v: vector
Vector3 operator/( const Vector3 & a, float k )
{
	Vector3 v( a.x /k, a.y /k, a.z /k);
	return v;
}
// v -= w
void operator-=( Vector3 &v, const Vector3 &w){				
	v.x -= w.x;
	v.y -= w.y;
	v.z -= w.z;
}
// v += w
void operator+=( Vector3 &v, const Vector3 &w){				
	v.x += w.x;
	v.y += w.y;
	v.z += w.z;
}
// v = s*v
void operator*=( Vector3 &v, const float& s){				
	v.x *= s;
	v.y *= s;
	v.z *= s;
}
// v = v / s
void operator/=( Vector3 &v, const float &s){
	v.x /= s;
	v.y /= s;
	v.z /= s;
}

Vector3 add( Vector3 & a, Vector3 & b)
{
	Vector3 v( a.x + b.x, a.y + b.y, a.z + b.z );
	return v;
}
Vector3 subtract( Vector3 & a, Vector3 & b)
{	
	Vector3 v( a.x - b.x, a.y - b.y, a.z - b.z );
	return v;
}
float dotProduct( const Vector3 & a, const Vector3 & b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}
Vector3 scale( float k, Vector3 & a)
{
	Vector3 v( k * a.x, k * a.y, k * a.z );
	return v;
}
Vector3 negate( Vector3 & a)
{
	Vector3 v( -a.x, -a.y, -a.z );
	return v;
}
Vector3 crossProduct(const Vector3 & a, const Vector3 & b)
{
//Vec3 v;
//v[0] = a[1] * b[2] - a[2] * b[1];
//v[1] = a[2] * b[0] - a[0] * b[2];
//v[2] = a[0] * b[1] - a[1] * b[0];

	Vector3 v( a[1] * b[2] - a[2] * b[1],
			a[2] * b[0] - a[0] * b[2],
			a[0] * b[1] - a[1] * b[0] );
	return v;
}
Vector3 normalize( Vector3 & a)
{
	Vector3 v = a;

	float len2 = a.x*a.x + a.y*a.y + a.z*a.z;
	if( len2 != 0.0 ){
		len2 = sqrt(len2);
		v.x /= len2;
		v.y /= len2;
		v.z /= len2;
	}
	return v;
}
float length( Vector3 & a)
{
	float len2 = a.x*a.x + a.y*a.y + a.z*a.z;
	if( len2 != 0.0 ){
		return sqrt(len2);
	}
	else
		return len2;
}




/* ÂÂª©¥» */
void VecAdd( Vector3 &a, Vector3 &b, Vector3 &result )
{
	for( int i = 0; i < 3; i++ ){
		result[i] = a[i] + b[i];
	}
}
void VecSub( Vector3 &a, Vector3 &b, Vector3 &result )
{
	for( int i = 0; i < 3; i++ ){
		result[i] = a[i] - b[i];
	}	
}
void VecScale( Vector3 &a, float s, Vector3 &result )
{
	for( int i = 0; i < 3; i++ )
		result[i] = a[i] * s;
}
float VecDot( Vector3 &a, Vector3 &b )
{
	float r = 0.0;
	for(int i = 0; i<3; i++){
		r += a[i] * b[i];
	}
	return r;
}
void VecCross( Vector3 &a, Vector3 &b, Vector3 &result )
{
	result[0] = a[1] * b[2] - a[2] * b[1];
	result[1] = a[2] * b[0] - a[0] * b[2];
	result[2] = a[0] * b[1] - a[1] * b[0];
}
float VecLength( Vector3 &a )
{
	float len2 = a[0]*a[0] + a[1]*a[1] + a[2]*a[2];
	if( len2 != 0.0 ){
		len2 = sqrt( len2 );
	}
	return len2;
}
void NormalizeVec( Vector3 &a )
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
void NegateVec( Vector3 &a )
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