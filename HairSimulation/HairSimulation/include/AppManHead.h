#ifndef _APP_MANHEAD_H_
#define _APP_MANHEAD_H_

#include "AppPrerequisites.h"
#include "ApplicationObject.h"

class ManHead : public ApplicationObject
{
    protected:
        void setUp(const String& name);
    public:
        ManHead(const String& name);
        ~ManHead();
};

#endif