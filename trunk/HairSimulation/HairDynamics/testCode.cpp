//～～～～～～～～～～～～～～～～～～～～～～～～～
//～～～～～～～ 先測試寫好的小函式 ～～～～～～～～
//～～～～～～～～～～～～～～～～～～～～～～～～～

#if 0
#include	<ciostream>
#include	<cmath>
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
#include	"Vector3.h"

int main( int argc, char **argv )
{
	cout << sqrt( 2.0 ) << endl;

	AppVector3 v( 0, 0, 0 ), v1( 50, 0, 0 ), v2( 50, 50, 0 ), C( 55, 10, 0 ), P( 50, 10, 0 );
	AppVector3 N( 0, 0, 1 );
	float R = 5.0001;
	double temp = 1;
	//cout << (sphereEdge3D( v1, v2, C, R ) ? "Y":"N" ) << endl;
	//cout << (pointInsideTri( v1, v2, v, P ) ? "Y":"N" ) << endl;
	cout << (sphereTri3D( v, v1, v2, N, C, R ) ? "Y":"N" ) << endl;
	
	//cout << (ballTri_v0( v, v1, v2, C, R, &temp )?"Y ":"N ")<< R - temp << endl;
	....
	return 0;
}
#endif