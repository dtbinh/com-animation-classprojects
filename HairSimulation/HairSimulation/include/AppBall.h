#ifndef _APP_BALL_H_
#define _APP_BALL_H_

#include "AppPrerequisites.h"
#include "ApplicationObject.h"

class Ball : public ApplicationObject
{
protected:
	Real mRadius;
	void setUp(const String& name);
public:
	Ball(const String& name, Real radius);
	~Ball();
}; 

#endif