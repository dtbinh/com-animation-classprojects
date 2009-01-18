#ifndef _APP_BOX_H_
#define _APP_BOX_H_

#include "AppPrerequisites.h"
#include "ApplicationObject.h"

class AppBox : public ApplicationObject
{
protected:
	Vector3 mDimensions;
	void setUp(const String& name);
public:
	static int cNumAppBox;
	AppBox();
	AppBox(const String& name, Real width, Real height, Real depth);
	~AppBox();
	void setMaterialName(const String& matName);
	void setPositionByEnds(const Vector3& a, const Vector3& b);
}; 

#endif