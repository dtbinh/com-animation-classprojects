#ifndef _APP_UTILITIES_H_
#define _APP_UTILITIES_H_

#include "AppPrerequisites.h"
#include <Ogre.h>

using namespace Ogre;

class Utilities
{
public:
//--------------Static methods----------------------------------------
/** Retrieve vertex data */
static 
void getMeshInformation(const Ogre::MeshPtr mesh,
													size_t &vertex_count,
													Ogre::Vector3* &vertices,
													size_t &index_count,
													unsigned long* &indices,
													const Ogre::Vector3 &position = Vector3::ZERO,
													const Ogre::Quaternion &orient = Quaternion::IDENTITY,
													const Ogre::Vector3 &scale = Vector3::UNIT_SCALE);
static
float getArea(Vector3& p1, Vector3& p2, Vector3& p3);

static
bool isInList(Vector3& v, std::vector<Vector3> vertexList);

};
#endif