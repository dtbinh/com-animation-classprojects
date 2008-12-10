/*
	skeleton.h

	Definition of the skeleton. 

    Written by  Jehee Lee
*/

#ifndef _VECTOR_H
#define _VECTOR_H


class vector3
{
    // negation
    friend vector3    operator-( vector3 const& );

    // addtion
    friend vector3    operator+( vector3 const&, vector3 const& );

    // subtraction
    friend vector3    operator-( vector3 const&, vector3 const& );

    // dot product
    friend float    operator%( vector3 const&, vector3 const& );

    // cross product
    friend vector3    operator*( vector3 const&, vector3 const& );

    // scalar Multiplication
    friend vector3    operator*( vector3 const&, float );

    // scalar Division
    friend vector3    operator/( vector3 const&, float );


    friend float    len( vector3 const& );
    friend vector3	normalize( vector3 const& );

	friend vector3       interpolate( float, vector3 const&, vector3 const& );

    friend float       angle( vector3 const&, vector3 const& );

  // member functions
  public:
    // constructors
    vector3() {}
    vector3( float x, float y, float z ) { p[0]=x; p[1]=y; p[2]=z; }
    vector3( float a[3] ) { p[0]=a[0]; p[1]=a[1]; p[2]=a[2]; }
	~vector3() {};

    // inquiry functions
    float& operator[](int i) { return p[i];}

    float x() const { return p[0]; };
    float y() const { return p[1]; };
    float z() const { return p[2]; };
    void   getValue( float d[3] ) { d[0]=p[0]; d[1]=p[1]; d[2]=p[2]; }
    void   setValue( float d[3] ) { p[0]=d[0]; p[1]=d[1]; p[2]=d[2]; }

	float getValue( int n ) const { return p[n]; }
	vector3 setValue( float x, float y, float z )
								   { p[0]=x, p[1]=y, p[2]=z; return *this; }
	float setValue( int n, float x )
								   { return p[n]=x; }

	float length() const;

    // change functions
    void set_x( float x ) { p[0]=x; };
    void set_y( float x ) { p[1]=x; };
    void set_z( float x ) { p[2]=x; };

	//data members
    float p[3]; //X, Y, Z components of the vector
};


#endif
