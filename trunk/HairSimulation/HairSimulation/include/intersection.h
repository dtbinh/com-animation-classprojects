#ifndef _INTERSECTION_H_
#define _INTERSECTION_H_

#include <Ogre.h>

using namespace Ogre;

bool CheckLineBox( Vector3 L1, Vector3 L2, Vector3 B1, Vector3 B2, Vector3 &HitP );
bool CheckLineTri( const Vector3 &L1, const Vector3 &L2, const Vector3 &PV1, const Vector3 &PV2, const Vector3 &PV3, Vector3 &HitP );
bool PointInFrustum( const Vector3 &P, Vector3 Normals[4], Vector3 Points[8] );
bool TriInFrustum( Vector3 vTri[3], Vector3 Normals[4], Vector3 Points[8] );
bool LineInFrustum( const Vector3 &LP1, const Vector3 &LP2, Vector3 Normals[4], Vector3 Points[8] );

#endif