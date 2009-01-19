#include	"particle.h"
/****************************************************************************/
/* Constructors */
	Particle::Particle()
		:invM( 1.0f )
	{
		status = FREE;
		x = Vector3( 0, 0, 0 );
		v = Vector3( 0, 0, 0 );
		a = Vector3( 0, 0, 0 );
		// @todo cState = Free
	}

	Particle::Particle( const Particle &p )
		:x(p.x), v(p.v), a(p.a), invM(p.invM)
	{
		//x = p.x;
		//v = p.v;
		//a = p.a;
		//invM = p.invM;
		// @todo this->cState = p.cState
		status = p.status;
		hitTri = p.hitTri;
	}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~ Strand class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void Strand::buildBox()
	{
		assert( pList.size >= 2 );
		//~~~ 找出 Bounding Box 的 max, min ~~~~~~~~~~~~~~~~~~~~~
		//跳過第一個 particle 因為第一個 particle 是黏在 mesh 上
		
		Vector3 max = pList[1].x;
		Vector3 min = pList[1].x;
		for( unsigned int i = 2; i < pList.size(); ++i ){
			for( int j = 0; j < 3; ++j ){
				if( pList[i].x[j] > max[j] ){
					max[j] = pList[i].x[j];
				}else if( pList[i].x[j] < min[j] ){
					min[j] = pList[i].x[j];
				}
			}
		}
		assert( max[0] >= min[0] );
		assert( max[1] >= min[1] );
		assert( max[2] >= min[2] );
		for( int i = 0; i < 3; ++i ){
			min[i] -= P_BALL_R;
			max[i] += P_BALL_R;
		}
		m_box.bounds[0] = min;
		m_box.bounds[1] = max;
		m_box.center = ( max + min ) / 2;
		m_box.dim = ( max - min ) / 2;
	}

	void Strand::updateBox()
	{
		//跳過第一個 particle 因為第一個 particle 是黏在 mesh 上
		std::vector<Particle>::const_iterator pPtr = pList.begin() + 1;
		Vector3 max = pPtr->x;
		Vector3 min = pPtr->x;
		for( pPtr = pPtr + 1; pPtr != pList.end(); pPtr++ ){
			for( int j = 0; j < 3; ++j ){
				if( pPtr->x[j] > max[j] ){
					max[j] = pPtr->x[j];
				}else if( pPtr->x[j] < min[j] ){
					min[j] = pPtr->x[j];
				}
			}
		}
		assert( max[0] >= min[0] );
		assert( max[1] >= min[1] );
		assert( max[2] >= min[2] );
		for( int i = 0; i < 3; ++i ){
			min[i] -= P_BALL_R;
			max[i] += P_BALL_R;
		}
		m_box.bounds[0] = min;
		m_box.bounds[1] = max;
		m_box.center = ( max + min ) / 2;
		m_box.dim = ( max - min ) / 2;
	}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/****************************************************************************/
/*   Spring																    */
/****************************************************************************/
	Spring::Spring()
		:len0( 1.0f ), k( 10 ), kd( 0 )
	{
	}

	Spring::Spring( const Spring &spr )
	{
		this->len0 = spr.len0;
		this->k = spr.k;
		this->kd = spr.kd;
		this->pIndex[0] = spr.pIndex[0];
		this->pIndex[1] = spr.pIndex[1];
	}

	bool Spring::isOverStretched( const std::vector< Particle > &particleArray )
	{
		const Vector3 & x1 = particleArray[ pIndex[0] ].x;
		const Vector3 & x2 = particleArray[ pIndex[1] ].x;
		DP L = x1.distance( x2 );
		DP ratio = (L - len0 )/len0;
		if( ratio > MAX_STRETCH_RATIO ){
			return true;
		}
		return false;
	}

	// apply force/accel to particles
	void Spring::applyAccel2particle( std::vector< Particle> &particleArray )
	{
		Particle & p1 = particleArray[ pIndex[0] ];
		Particle & p2 = particleArray[ pIndex[1] ];

		Vector3 L = p2.x - p1.x;
		DP f = -k *( L.length() - len0 );	// f = -k * dL
		L.normalize();
		Vector3 dV = p2.v - p1.v;			// spring's damper term
		f += -kd * dotProduct( dV, L );
		L = L * f;		// L = F now
		p2.a += L * p2.invM;
		p1.a += -L * p1.invM;
	}

#if 0
	bool Spring::isOverStretched( const Vector3 p1, const Vector3 p2 )
	{
		DP L = p1.distance( p2 );
		DP ratio = (L - len0 )/len0;
		if( ratio > MAX_STRETCH_RATIO ){
			return true;
		}
		return false;
	}

	//作用在 p2 上的 forec
	Vector3 Spring::getForce( const Vector3 p1, const Vector3 p2 )
	{
		Vector3 L = p2 - p1;
		DP f = k *( L.length() - len0 );
		L.normalize();
		return Vector3( f * L );
	}
#endif