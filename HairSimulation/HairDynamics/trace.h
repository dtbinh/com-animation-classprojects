#include	<cmath>
#include	"Vector3.h"
#include	"ray.h"
#include	"rayTriangle.h"
#include	"camera.h"
#include	"mesh.h"
//#include	"GLgl.h"
//extern Camera camera;		//攝影機/相機
#include	"glut.h"

extern int winW, winH;		// 視窗的大小

extern LightSource light;	//光源
extern Camera camera;		//攝影機/相機

extern std::vector< mesh * > objectList; //場景中的物體

#define EPSILON 1E-5
// 函式 prototype
Vector3 directLightingColor( const Vector3 &point, const Vector3 &n, Face *iFacePtr, const Vector3 &inRayDir );
Vector3 computeColor( const Vector3 &point, const Vector3 &n, Face *iFace, mesh *iMesh, const Vector3 &inRayDir )
;
Vector3 reflectedLightColor( const Vector3 &point, const Vector3 &n, const Vector3 &inRayDir );
void display1();	//ray trace 版本的 display
void display_debug();