#include	<cmath>
#include	"Vector3.h"

//~~~~~ prototype ~~~~~~
bool sphereTri3D( const Vector3 & A, const Vector3 & B, const Vector3 & C, const Vector3 & N, const Vector3 & P, float R );

// EIRT
bool sphereEdge3D( const Vector3 & A, const Vector3 & B, const Vector3 &P, float R )
{
	Vector3 ap = P - A;
	Vector3 bp = P - B;
	float ap2 = ap.squaredLength();
	float bp2 = bp.squaredLength();
	float r2 = R * R;
	if( ap2 <= r2 || bp2 <= r2 ){
		return true;
	}else{
		Vector3 ab = B - A;
		float x = dotProduct( ab, ap );
		if( x < 0 ){
			return false;
		}else{
			float ab2 = ab.squaredLength();
			float d22 = x * x / ab2;
			if( d22 > ab2 ){
				return false;
			}
			if( ap2 - d22 <= r2 ){
				return true;
			}else{
				return false;
			}
		}
	}
}
bool sphereEdge3D_v2( const Vector3 & A, const Vector3 & B, const Vector3 &P, float r2 )
{
	Vector3 ap = P - A;
	Vector3 bp = P - B;
	float ap2 = ap.squaredLength();
	float bp2 = bp.squaredLength();
	
	if( ap2 <= r2 || bp2 <= r2 ){
		return true;
	}else{
		Vector3 ab = B - A;
		float x = dotProduct( ab, ap );
		if( x < 0 ){
			return false;
		}else{
			float ab2 = ab.squaredLength();
			float d22 = x * x / ab2;
			if( d22 > ab2 ){
				return false;
			}
			if( ap2 - d22 <= r2 ){
				return true;
			}else{
				return false;
			}
		}
	}
}

// http://www.ziggyware.com/readarticle.php?article_id=78
bool pointInsideTri( const Vector3 & A, const Vector3 & B, const Vector3 &C, const Vector3 &P )
{
	const float Epsilon = 1e-5f;
	int pos = 0, neg = 0;
	Vector3 ab = B-A, bc = C-B, ca = A-C;
	Vector3 N = crossProduct( ab, bc) ;
	
	Vector3 n = crossProduct( ab, N );
	float halfPlane = dotProduct( P, n ) - dotProduct( A, n );
	if( halfPlane > Epsilon ){
		pos++;
	}else if( halfPlane < -Epsilon ){
		neg++;
	}
	if( (pos && neg) != 0 )	return false;	//early out

	n = crossProduct( bc, N );
	halfPlane = dotProduct( P, n ) - dotProduct( B, n );
	if( halfPlane > Epsilon ){
		pos++;
	}else if( halfPlane < -Epsilon ){
		neg++;
	}
	if( (pos && neg) != 0 )	return false;	//early out

	n = crossProduct( ca, N );
	halfPlane = dotProduct( P, n ) - dotProduct( C, n );
	if( halfPlane > Epsilon ){
		pos++;
	}else if( halfPlane < -Epsilon ){
		neg++;
	}
	if( (pos && neg) != 0 )	return false;	//early out

	if( (pos && neg) == 0)	return true;

	
	return false;
}

bool sphereTri3D( const Vector3 & A, const Vector3 & B, const Vector3 & C, const Vector3 & N, 
				 const Vector3 & P, float R )
{
	// check 3 edge
	if( sphereEdge3D( A, B, P, R ) ){
		return true;
	}else if( sphereEdge3D( B, C, P, R ) ){
		return true;
	}else if( sphereEdge3D( C, A, P, R ) ){
		return true;
	}else{
		Vector3 pa = P-A;
		float pqLen = dotProduct( N, pa );	// N is normalized
		//if( pqLen > R ){	// |PQ| > R =>這樣寫是錯的=> 因為 dotProduct 可以有正有負 abs( DotProduct() )
		if( abs( pqLen ) > R ){
			return false;
		}else{
			Vector3 Q = P + N * pqLen;
			return pointInsideTri( A, B, C, Q );
		}
	}
}

bool sphereTri3D_v2( const Vector3 & A, const Vector3 & B, const Vector3 & C, const Vector3 & N, 
				 const Vector3 & P, float R2 )
{
	// check 3 edge
	if( sphereEdge3D_v2( A, B, P, R2 ) ){
		return true;
	}else if( sphereEdge3D_v2( B, C, P, R2 ) ){
		return true;
	}else if( sphereEdge3D_v2( C, A, P, R2 ) ){
		return true;
	}else{
		Vector3 pa = P-A;
		float pqLen = dotProduct( N, pa );	// N is normalized
		//if( pqLen > R ){	// |PQ| > R =>這樣寫是錯的=> 因為 dotProduct 可以有正有負 abs( DotProduct() )
		if( pqLen * pqLen > R2 ){
			return false;
		}else{
			Vector3 Q = P + N * pqLen;
			return pointInsideTri( A, B, C, Q );
		}
	}
}
