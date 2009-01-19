/********************************************************
*	Vec3 class
*	三度空間的向量
*
*	修改日期: 
*	2008-12-06
*   2008-12-08 發現內積寫錯了 operator*　-> 改正；
*   2009-1-12 
*		為了和 OGRE AppVector3 match
*		cross() => crossProduct()
*		dot() => dotProduct()
*		拿掉 dot 內積的 overloaded operator
*********************************************************/
#include	<iostream>
#ifndef	VEC3_H
#define	VEC3_H

class AppVector3

{
	friend AppVector3 operator-( AppVector3 &);				// -v;
	friend AppVector3 operator-( const AppVector3 &, const AppVector3 & );	// v - w
	friend AppVector3 operator+( const AppVector3 &, const AppVector3 & );	// v + w
	//friend float operator*( const AppVector3 &, const AppVector3 & );	// v dot w
	friend AppVector3 operator*( float , const AppVector3 & );	// s* v		s:scalar
	friend AppVector3 operator*( const AppVector3 &, float );		// v * s	s:scalar
	friend AppVector3 operator/( const AppVector3 &, float );		// v / s	s:scalar
	friend void operator-=( AppVector3 &v, const AppVector3 &w);
	friend void operator+=( AppVector3 &v, const AppVector3 &w);
	friend void operator*=( AppVector3 &v, const float& s);
	friend void operator/=( AppVector3 &v, const float &s);

	friend AppVector3 add( AppVector3 &, AppVector3 &);
	friend AppVector3 subtract( AppVector3 &, AppVector3 & );
	friend float dotProduct( const AppVector3 &, const AppVector3 & );	// v dot w
	friend AppVector3 scale( float , AppVector3 & );
	friend AppVector3 negate( AppVector3 & );
	friend AppVector3 crossProduct(const AppVector3 &, const AppVector3 &);	// v cross w
	friend AppVector3 normalize( AppVector3 &);
	friend float length( AppVector3 & );

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
	AppVector3 (float *v) 
	{
		for (int i=0;i<3;i++)
			ptr[i] = v[i];
	}
	AppVector3( float x, float y, float z )
	{
		ptr[0] = x;
		ptr[1] = y;
		ptr[2] = z;
	}
	AppVector3()
	{
		ptr[0] = ptr[1] = ptr[2] = 0;
	}
	// copy constructor: T( const T &)
	AppVector3( const AppVector3 & copy )
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

	inline float distance(const AppVector3& rhs) const
	{
	    return (*this - rhs).length();
	}

	float squaredLength () const;
	float squaredDistance(const AppVector3& rhs) const;



};

void VecAdd( AppVector3 &a, AppVector3 &b, AppVector3 &result );
void VecSub( AppVector3 &a, AppVector3 &b, AppVector3 &result );
float VecDot( AppVector3 &a, AppVector3 &b );
void VecCross( AppVector3 &a, AppVector3 &b, AppVector3 &result );
void VecScale( AppVector3 &a, float s, AppVector3 &result );
float VecLength( AppVector3 &a );
void NormalizeVec( AppVector3 &a );
void NegateVec( AppVector3 &a );

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