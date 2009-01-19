/********************************************************
*	Vec3 class
*	三度空間的向量
*
*	修改日期: 
*	2008-12-06
*   2008-12-08 發現內積寫錯了 operator*　-> 改正；
*   2009-1-12 
*		為了和 OGRE Vector3 match
*		cross() => crossProduct()
*		dot() => dotProduct()
*		拿掉 dot 內積的 overloaded operator
*********************************************************/
#include	<iostream>
#ifndef	VEC3_H
#define	VEC3_H

class Vector3

{
	friend Vector3 operator-( Vector3 &);				// -v;
	friend Vector3 operator-( const Vector3 &, const Vector3 & );	// v - w
	friend Vector3 operator+( const Vector3 &, const Vector3 & );	// v + w
	//friend float operator*( const Vector3 &, const Vector3 & );	// v dot w
	friend Vector3 operator*( float , const Vector3 & );	// s* v		s:scalar
	friend Vector3 operator*( const Vector3 &, float );		// v * s	s:scalar
	friend Vector3 operator/( const Vector3 &, float );		// v / s	s:scalar
	friend void operator-=( Vector3 &v, const Vector3 &w);
	friend void operator+=( Vector3 &v, const Vector3 &w);
	friend void operator*=( Vector3 &v, const float& s);
	friend void operator/=( Vector3 &v, const float &s);

	friend Vector3 add( Vector3 &, Vector3 &);
	friend Vector3 subtract( Vector3 &, Vector3 & );
	friend float dotProduct( const Vector3 &, const Vector3 & );	// v dot w
	friend Vector3 scale( float , Vector3 & );
	friend Vector3 negate( Vector3 & );
	friend Vector3 crossProduct(const Vector3 &, const Vector3 &);	// v cross w
	friend Vector3 normalize( Vector3 &);
	friend float length( Vector3 & );

public:
	union{
		struct{
			float x, y, z;
		};
		/*struct{
			float r, g, b;
		};*/
		float ptr[3];
	};

	/** constructors */
	Vector3 (float *v) 
	{
		for (int i=0;i<3;i++)
			ptr[i] = v[i];
	}
	Vector3( float x, float y, float z )
	{
		ptr[0] = x;
		ptr[1] = y;
		ptr[2] = z;
	}
	Vector3()
	{
		ptr[0] = ptr[1] = ptr[2] = 0;
	}
	// copy constructor: T( const T &)
	Vector3( const Vector3 & copy )
	{
		x = copy.x;
		y = copy.y;
		z = copy.z;
	}
	//subscript operator for non-const objects returns modifiable lvalue
	float& operator[](int index)
	{
		return ptr[index];	//reference
	}
	float operator[]( int index ) const
	{
		return ptr[index];	//copy of this element
	}

	inline void printInfo()
	{
		printf("(%.3f,%.3f,%.3f)", x, y, z);
	}

	inline void printInfo( FILE * fp )
	{
		fprintf(fp,"(%.3f,%.3f,%.3f)", x, y, z);
	}
	float length() const;
	void normalize();

	inline float distance(const Vector3& rhs) const
	{
	    return (*this - rhs).length();
	}

	float squaredLength () const;
	float squaredDistance(const Vector3& rhs) const;



};

void VecAdd( Vector3 &a, Vector3 &b, Vector3 &result );
void VecSub( Vector3 &a, Vector3 &b, Vector3 &result );
float VecDot( Vector3 &a, Vector3 &b );
void VecCross( Vector3 &a, Vector3 &b, Vector3 &result );
void VecScale( Vector3 &a, float s, Vector3 &result );
float VecLength( Vector3 &a );
void NormalizeVec( Vector3 &a );
void NegateVec( Vector3 &a );

class Vec4{
public:
	union{
		struct{
			float x, y, z, w;
		};
		struct{
			float r, g, b, a;
		};

		float xyzw[4];
		float ptr[4];
	};

	Vec4(){
		x = y = z = 0;
	};
	Vec4( float x1, float x2, float x3, float x4 ){
		x = x1; y = x2; z = x3; w = x4;
	}

	float& operator[](int index)
	{
		return xyzw[index];
	}
};

void VecAdd( Vec4 &a, Vec4 &b, Vec4 &result );
void VecSub( Vec4 &a, Vec4 &b, Vec4 &result );
void VecScale( Vec4 &a, float s, Vec4 &result );
void NormalizeVec( Vec4 &a );

#endif