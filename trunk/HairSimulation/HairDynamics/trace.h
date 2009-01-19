#include	<cmath>
#include	"Vector3.h"
#include	"ray.h"
#include	"rayTriangle.h"
#include	"camera.h"
#include	"mesh.h"
//#include	"GLgl.h"
//extern Camera camera;		//��v��/�۾�
#include	"glut.h"

extern int winW, winH;		// �������j�p

extern LightSource light;	//����
extern Camera camera;		//��v��/�۾�

extern std::vector< mesh * > objectList; //������������

#define EPSILON 1E-5
// �禡 prototype
Vector3 directLightingColor( const Vector3 &point, const Vector3 &n, Face *iFacePtr, const Vector3 &inRayDir );
Vector3 computeColor( const Vector3 &point, const Vector3 &n, Face *iFace, mesh *iMesh, const Vector3 &inRayDir )
;
Vector3 reflectedLightColor( const Vector3 &point, const Vector3 &n, const Vector3 &inRayDir );
void display1();	//ray trace ������ display
void display_debug();