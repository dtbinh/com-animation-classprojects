#ifndef MAIN_H
#define MAIN_H

#include	<ctime>
#include	<iostream>
#include	<cstdlib>
#include	<cfloat>
#include	<cmath>
using namespace std;
//#include	"glew.h"
//#include	"wglew.h"
#include	"glaux.h"
#include	"glut.h"

#include	"mesh.h"
#include	"Vector3.h"
//#include	"ray.h"
//#include	"rayTriangle.h"
#include	"camera.h"
//#include	"trace.h"
#define		TEX_NUM 2
#define		MIPMAP

//#define EPSILON 1E-5


enum ChangeSettingType{
	LIGHT = 0, MATERIAL = 1
};

#include	"line.h"
#include	"strandSystem.h"

// ¨ç¦¡ prototype
Line makeALine( const Face &face, float u, float v );
void addLinesOnFace( const Face &face, std::vector<Line> *lineArray );
//void LoadTexture( int textureIndex, char *fileName );

void setLight();
void display();
void reshape( GLsizei, GLsizei );
void keyboard( unsigned char key, int x, int y );
void processSpecialKeys( int key, int x, int y);
//void mouse(int button, int state, int x, int y);
//void mouseMotion(int x, int y);
void idle();
// screen shot
void takePicture();
GLubyte * ReadDIBitmap(BITMAPINFO &info);
bool SaveDIBitmap( const char *fileName, BITMAPINFO *info, GLubyte *bits );

#endif