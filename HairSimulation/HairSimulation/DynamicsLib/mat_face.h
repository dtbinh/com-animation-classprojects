#ifndef MAT_FACE_H
#define MAT_FACE_H

#include	<Ogre.h>
using namespace Ogre;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~ material class for mesh, Face class ~~~~~~~~~~~~~~~~~~~~~~~~~~
class material
{
public:
	float Ka[4];
	float Kd[4];
	float Ks[4];
	float Ns;		// shiness

	material()
	{ 
		for (int i=0;i<4;i++)
			Ka[i] = Kd[i] = Ks[i] = 1;
		Ns = 0;
	}

	inline void printInfo()
	{
		printf("Ka(%g,%g,%g,%g)\t", Ka[0], Ka[1], Ka[2], Ka[3]);
		printf("Kd(%g,%g,%g,%g)\t", Kd[0], Kd[1], Kd[2], Kd[3]);
		printf("Ks(%g,%g,%g,%g)\t", Ks[0], Ks[1], Ks[2], Ks[3]);
		printf("Ns %g\n", Ns);
	}
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Face class:vertex,normal,tex-coord,material資料都放在pointer裡
class Face{
public:
	Vector3 *vPtr[3];		//vertex data指標
	Vector3 *nPtr[3];		//normal data指標
	Vector3 *tPtr[3];		//貼圖data指標
	Vector3 *fnPtr;		//face normal指標
	Vector3 *cPtr;			// tri centroid ptr=>to be deleted
	material *materialPtr;//material data 指標

	Face();
	
	Face( const Face & );
/*
	inline void printInfo()
	{
		if( vPtr[0] != 0 ){
			printf("------------face data------------\n");
			printf("v:");
			vPtr[0]->printInfo();
			vPtr[1]->printInfo();
			vPtr[2]->printInfo();
			printf("\n");
			printf("n:");
			nPtr[0]->printInfo();
			nPtr[1]->printInfo();
			nPtr[2]->printInfo();
			printf("\n");
			printf("mat:");
			materialPtr->printInfo();
			printf("---------------------------------\n");
		}
	}
	*/
};
#endif