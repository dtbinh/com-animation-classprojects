#ifndef _APP_OGREHEAD_H_
#define _APP_OGREHEAD_H_

#include "AppPrerequisites.h"
#include "ApplicationObject.h"

class OgreHead : public ApplicationObject
{
    protected:
        void setUp(const String& name);
    public:
        OgreHead(const String& name);
        ~OgreHead();
};

#endif