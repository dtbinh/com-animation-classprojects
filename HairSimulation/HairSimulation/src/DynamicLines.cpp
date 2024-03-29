#include "DynamicLines.h"
#include <Ogre.h>
#include <cassert>
#include <cmath>

using namespace Ogre;

enum {
  POSITION_BINDING,
  TEXCOORD_BINDING
};

DynamicLines::DynamicLines(OperationType opType)
{
/*
  initialize(opType,false);
  setMaterial("BaseWhiteNoLighting");
  mDirty = true;
*/
	initDynamicLines(Ogre::ColourValue::Red, opType);
}

DynamicLines::DynamicLines(ColourValue colour, OperationType opType)
{
  initDynamicLines(colour, opType);
}

DynamicLines::~DynamicLines()
{
}

void DynamicLines::initDynamicLines(Ogre::ColourValue colour, OperationType opType)
{
  initialize(opType,false);
  static int matIndex = 0;
  String matName = "DL" + StringConverter::toString(matIndex++);
  MaterialPtr materialPtr = MaterialManager::getSingleton().create(matName,
              ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  materialPtr->setAmbient(colour);

  setMaterial(matName);
 
  mDirty = true;
}

void DynamicLines::setColour(Ogre::ColourValue colour)
{
  getMaterial()->setAmbient(colour);
}


void DynamicLines::setOperationType(OperationType opType)
{
  mRenderOp.operationType = opType;
}

RenderOperation::OperationType DynamicLines::getOperationType() const
{
  return mRenderOp.operationType;
}

void DynamicLines::addPoint(const Vector3 &p)
{
   mPoints.push_back(p);
   mDirty = true;
}
void DynamicLines::addPoint(Real x, Real y, Real z)
{
   mPoints.push_back(Vector3(x,y,z));
   mDirty = true;
}


void DynamicLines::addSegment(const Ogre::Vector3 &p1, const Ogre::Vector3 &p2)
{
  mPoints.push_back(p1);
  mPoints.push_back(p2);
  mDirty = true;
}

const Vector3& DynamicLines::getPoint(unsigned short index) const
{
   assert(index < mPoints.size() && "Point index is out of bounds!!");
   return mPoints[index];
}
unsigned short DynamicLines::getNumPoints(void) const
{
  return (unsigned short)mPoints.size();
}
void DynamicLines::setPoint(unsigned short index, const Vector3 &value)
{
  assert(index < mPoints.size() && "Point index is out of bounds!!");

  mPoints[index] = value;
  mDirty = true;
}
void DynamicLines::clear()
{
  mPoints.clear();
  mDirty = true;
}

void DynamicLines::update()
{
  if (mDirty) fillHardwareBuffers();
}

void DynamicLines::createVertexDeclaration()
{
  VertexDeclaration *decl = mRenderOp.vertexData->vertexDeclaration;
  decl->addElement(POSITION_BINDING, 0, VET_FLOAT3, VES_POSITION);
}

void DynamicLines::fillHardwareBuffers()
{
  int size = (int) mPoints.size();

  prepareHardwareBuffers(size,0);

  if (!size) { 
    mBox.setExtents(Vector3::ZERO,Vector3::ZERO);
    mDirty=false;
    return;
  }
  
  Vector3 vaabMin = mPoints[0];
  Vector3 vaabMax = mPoints[0];

  HardwareVertexBufferSharedPtr vbuf =
    mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);

  Real *prPos = static_cast<Real*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));
  {
   for(int i = 0; i < size; i++)
   {
      *prPos++ = mPoints[i].x;
      *prPos++ = mPoints[i].y;
      *prPos++ = mPoints[i].z;

      if(mPoints[i].x < vaabMin.x)
         vaabMin.x = mPoints[i].x;
      if(mPoints[i].y < vaabMin.y)
         vaabMin.y = mPoints[i].y;
      if(mPoints[i].z < vaabMin.z)
         vaabMin.z = mPoints[i].z;

      if(mPoints[i].x > vaabMax.x)
         vaabMax.x = mPoints[i].x;
      if(mPoints[i].y > vaabMax.y)
         vaabMax.y = mPoints[i].y;
      if(mPoints[i].z > vaabMax.z)
         vaabMax.z = mPoints[i].z;
   }
  }
  vbuf->unlock();

  mBox.setExtents(vaabMin, vaabMax);

  mDirty = false;
}

/*
void DynamicLines::getWorldTransforms(Matrix4 *xform) const
{
   // return identity matrix to prevent parent transforms
   *xform = Matrix4::IDENTITY;
}
*/
/*
const Quaternion &DynamicLines::getWorldOrientation(void) const
{
   return Quaternion::IDENTITY;
}

const Vector3 &DynamicLines::getWorldPosition(void) const
{
   return Vector3::ZERO;
}
*/