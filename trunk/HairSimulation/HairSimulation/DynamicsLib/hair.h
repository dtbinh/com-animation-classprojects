#ifndef HAIR_H
#define	HAIR_H

#include	<Ogre.h>	//用 OGRE 畫的時候應該不用加這行吧
using namespace Ogre;
#include	<vector>

/* interface */
class Hair{

public:
	std::vector< Vector3 > pts;	//points 
};

#endif