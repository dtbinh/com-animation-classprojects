#include <math.h>
#include <stdio.h>
#include "transform.h"
#include "types.h"

#include "vector.h"

vector3 operator-( vector3 const& a, vector3 const& b )
{
    vector3 c;

    c.p[0] = a.p[0] - b.p[0];
    c.p[1] = a.p[1] - b.p[1];
    c.p[2] = a.p[2] - b.p[2];

    return c;
}

vector3 operator+( vector3 const& a, vector3 const& b )
{
    vector3 c;

    c.p[0] = a.p[0] + b.p[0];
    c.p[1] = a.p[1] + b.p[1];
    c.p[2] = a.p[2] + b.p[2];

    return c;
}

vector3 operator/( vector3 const& a, float b )
{
    vector3 c;

    c.p[0] = a.p[0] / b;
    c.p[1] = a.p[1] / b;
    c.p[2] = a.p[2] / b;

    return c;
}

//multip
vector3 operator*( vector3 const& a, float b )
{
    vector3 c;

    c.p[0] = a.p[0] * b;
    c.p[1] = a.p[1] * b;
    c.p[2] = a.p[2] * b;

    return c;
}


//cross prodact
vector3 operator*( vector3 const& a, vector3 const& b )
{
    vector3 c;

    c.p[0] = a.p[1]*b.p[2] - a.p[2]*b.p[1];
    c.p[1] = a.p[2]*b.p[0] - a.p[0]*b.p[2];
    c.p[2] = a.p[0]*b.p[1] - a.p[1]*b.p[0];

    return c;
}

//dot prodact
float operator%( vector3 const& a, vector3 const& b )
{
    return ( a.p[0]*b.p[0] + a.p[1]*b.p[1] + a.p[2]*b.p[2] );
}


vector3
interpolate( float t, vector3 const& a, vector3 const& b )
{
	return a*(1.0-t) + b*t;
}

float len( vector3 const& v )
{
    return sqrt( v.p[0]*v.p[0] + v.p[1]*v.p[1] + v.p[2]*v.p[2] );
}

float
vector3::length() const
{
    return sqrt( p[0]*p[0] + p[1]*p[1] + p[2]*p[2] );
}


float angle( vector3 const& a, vector3 const& b )
{
    return acos( (a%b)/(len(a)*len(b)) );
}


