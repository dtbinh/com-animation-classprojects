#include	"strandSystem.h"

using std::vector;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// �]�w strand �t��
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void StrandSystem::setup( vector<Line> &lineSet, mesh *meshPtr )
{
	setupStrand( lineSet );
	m_time = 0;
	m_stepMethod = RUNGE_KUTTA;

	setMeshObject( meshPtr );

	// setup strand's bounding box
	for( vector<Strand>::iterator sptr = m_strandList.begin();
		sptr != m_strandList.end(); ++sptr )
	{
		sptr->buildBox();
	}
}
void StrandSystem::updateStrandBoxes()
{
	for( vector<Strand>::iterator sptr = m_strandList.begin();
		sptr != m_strandList.end(); ++sptr )
	{
		sptr->updateBox();
	}
}
void StrandSystem::drawStrandBoxes()
{
	for( vector<Strand>::iterator sptr = m_strandList.begin();
		sptr != m_strandList.end(); ++sptr )
	{
		sptr->m_box.display();;
	}
}
void StrandSystem::setupStrand( vector<Line> &lineSet )
{	// �[ particle
	m_totalStrand = int(lineSet.size());
	for( unsigned int i = 0; i < lineSet.size(); ++i )
	{
		Strand aStrand;
		aStrand.pTotal = int( lineSet[i].pts.size());
		aStrand.sTotal = aStrand.pTotal - 1;
		DP inverseMass = 10;
		for( unsigned int j = 0; j < lineSet[i].pts.size(); ++j )
		{
			//�[�W���I
			Particle particle;
			particle.x = lineSet[i].pts[j];
			particle.invM = inverseMass;
			//attach root of this strand to triangle surface
			// m = infinite, 1/m = 0
			if( j == 0 ){
				particle.invM = 0.0f;
			}
			aStrand.pList.push_back( particle );

			//�[�W�u®
			if( j != lineSet[i].pts.size() - 1 ){

				Spring spring;
				spring.setParticleIndex( j, j+1 );
				spring.setK( 500 );
				spring.setLen0( lineSet[i].pts[j].distance(lineSet[i].pts[j+1]) );
				aStrand.sList.push_back( spring );
			}
		}
		m_strandList.push_back( aStrand );
	}// close for-loop
}

//�] ODE step ��k
void StrandSystem::setStepMethod( int m )
{
	assert( m >=0 && m <= 2 );
	m_stepMethod = m;
}

//�t�ήɶ��e�i�@�� timeStep 
void StrandSystem::step( DP timeStep )
{
	for( vector<Strand>::iterator sptr = m_strandList.begin();
		sptr != m_strandList.end(); ++sptr )
	{
		RungeKutta( *sptr, timeStep );
		//switch( m_stepMethod ){
		//	case EULER:
		//		Euler( *sptr, timeStep );
		//		break;
		//	case MID_POINT:
		//		midPoint( *sptr, timeStep );
		//		break;
		//	case RUNGE_KUTTA:
		//		RungeKutta( *sptr, timeStep );
		//		break;
		//	default:
		//		RungeKutta( *sptr, timeStep );
		//		break;
		//}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//	���@�U Collision Status �������T
#define	TEST_C_STATUS
#ifdef	TEST_C_STATUS
		testP2TriCollision_box_bvTree( *sptr, m_meshPtr );
#endif
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	}
	m_time += timeStep;	//�O���t�ήɶ�
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!~
// private method 
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!~	





//~~~~~~~~~~~~~ update Particle �b�o�� time step �� status ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void traversal( TreeNode *tptr, Strand & strand )
{
	//~~~~~~~ function prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bool sphereTri3D_v2( const Vector3 & A, const Vector3 & B, const Vector3 & C, const Vector3 & N, const Vector3 & P, float R2 );
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	if( tptr->box.boxBoxIntersect( strand.m_box ) ){
		
		if( tptr->left == NULL ){	// leaf node

			// for each particle
			for( vector<Particle>::iterator pPtr = strand.pList.begin() + 1; // �Ĥ@�� particle ���δ�=>�H�bmesh�W
				pPtr != strand.pList.end(); ++pPtr )
			{	
				if( (pPtr->status == FREE) && ( tptr->box.BallBoxIntersect( pPtr->x, P_BALL_R2 ) ) ){
					if( sphereTri3D_v2( *(tptr->facePtr->vPtr[0]), *(tptr->facePtr->vPtr[1]), *(tptr->facePtr->vPtr[2]), *(tptr->facePtr->fnPtr), pPtr->x, P_BALL_R2) ){
						//std::cout<< "intersected\t";
						pPtr->status = CONTACT;		//�� intersect �N�ܤ֤@�w�� contact
						pPtr->hitTri = &(*tptr->facePtr);

						float dir = dotProduct( pPtr->v, *(tptr->facePtr->fnPtr) );
						if( dir < 1e-4f ){			// v �]�¤T���Ϊ��ܴN�O collide
							pPtr->status = COLLIDE;
							pPtr->hitTri = &(*tptr->facePtr);
						}
					}
				}
			}
		}else{	// non-leaf
			traversal( tptr->left, strand );
			traversal( tptr->right, strand );
		}
	}
}

void StrandSystem::testP2TriCollision_box_bvTree( Strand & strand, mesh *meshPtr )		
{
	//~~~ prototype ~~~
	void traversal( TreeNode *tptr, Strand & strand );
	// !!���n upadet strand's bounding box
	strand.updateBox();
	// ��l�� status <= Free
	for( vector<Particle>::iterator pPtr = strand.pList.begin() + 1; // �Ĥ@�� particle ���δ�=>�H�bmesh�W
		pPtr != strand.pList.end(); ++pPtr )
	{	
		pPtr->status = FREE;
	}
	traversal( meshPtr->m_bvTree.root, strand );
}

void StrandSystem::testP2TriCollision_bvTree( Strand & strand, mesh *meshPtr )		
{
	for( vector<Particle>::iterator pPtr = strand.pList.begin() + 1; // �Ĥ@�� particle ���δ�=>�H�bmesh�W
		pPtr != strand.pList.end(); ++pPtr )
	{	
		pPtr->status = FREE;
		// R = 0.2f => R^2 = 0.04f
		Face *tptr = NULL;
		if( (tptr = meshPtr->m_bvTree.testBallTri_v2( pPtr->x, P_BALL_R2 )) != NULL ){
			//std::cout<< "intersected\t";
			pPtr->status = CONTACT;		//�� intersect �N�ܤ֤@�w�� contact
			pPtr->hitTri = &(*tptr);

			float dir = dotProduct( pPtr->v, *(tptr->fnPtr) );
			if( dir < 1e-4f ){			// v �]�¤T���Ϊ��ܴN�O collide
				pPtr->status = COLLIDE;
				pPtr->hitTri = &(*tptr);
			}
		}
	}
}
void StrandSystem::testP2TriCollision( Strand & strand, mesh *meshPtr )		
{
	//~~~~~~~ function prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bool sphereTri3D( const Vector3 & A, const Vector3 & B, const Vector3 & C, const Vector3 & N, 
					 const Vector3 & P, float R );
	bool pointInsideTri( const Vector3 & A, const Vector3 & B, const Vector3 &C, const Vector3 &P );
	bool sphereEdge3D( const Vector3 & A, const Vector3 & B, const Vector3 &P, float R );
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	for( vector<Particle>::iterator pPtr = strand.pList.begin() + 1; // �Ĥ@�� particle ���δ�=>�H�bmesh�W
		pPtr != strand.pList.end(); ++pPtr )
	{	
		pPtr->status = FREE;
		// �u��Ĥ@�� intersect ���T����=>�ܦhintersect�T���Ϊ�case�H��A��
		for( vector<Face>::iterator tptr = meshPtr->myFaceList.begin();
			tptr != meshPtr->myFaceList.end(); tptr++ )
		{
			if( sphereTri3D( *(tptr->vPtr[0]), *(tptr->vPtr[1]), *(tptr->vPtr[2]), *(tptr->fnPtr), pPtr->x, P_BALL_R) ){
				//std::cout<< "intersected\t";

				pPtr->status = CONTACT;		//�� intersect �N�ܤ֤@�w�� contact
				pPtr->hitTri = &(*tptr);

				float dir = dotProduct( pPtr->v, *(tptr->fnPtr) );
				if( dir < 1e-4f ){			// v �]�¤T���Ϊ��ܴN�O collide
					pPtr->status = COLLIDE;
					pPtr->hitTri = &(*tptr);
				}
				break;//�C�� particle �u�O�Ĥ@�� intersect ���T���ΡF
			}
		}
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// update particle's (v,a) with Status==Collide ,Contact
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void updateVA( Strand & strand )
{
	//int fcnt = 0, ccnt = 0, tcnt =0;	// �Ĥ@�� particle ���δ�=>�H�bmesh�W
	for( vector<Particle>::iterator pPtr = strand.pList.begin() +1;
		pPtr != strand.pList.end(); ++pPtr )
	{
		if( pPtr->status == COLLIDE ){
			Vector3 &N = *(pPtr->hitTri->fnPtr);
			//DP vDir = dotProduct( pPtr->v, N );	//���n���ϼu
			//if( vDir < 0 ){
			//	Vector3 Vn = vDir * N;
			//	Vector3 Vt = pPtr->v - Vn;
			//	pPtr->v = -1e-4 * Vn + Vt;
			//}
			//Vector3 Vn = dotProduct( pPtr->v, N ) * N;	//�S���n�ϼu
			//Vector3 Vt = pPtr->v - Vn;					//���ά� v ����V,�]���ˬdCollide�ɴN�P�_�L�F
			
			pPtr->v = pPtr->v - (dotProduct( pPtr->v, N ) * N);

			DP aDir =  dotProduct( pPtr->a, N );	//�� a ����V,���������� An
			if( aDir < 1e-5 ){					
				pPtr->a = pPtr->a - ( aDir * N );
			}
		}else if( pPtr->status == CONTACT ){
			Vector3 &N = *(pPtr->hitTri->fnPtr);
			DP vDir = dotProduct( pPtr->v, N );
			if( vDir < 1e-5 ){					//�� v ����V,���������� Vn
				//Vector3 Vn = vDir * N;
				//Vector3 Vt = pPtr->v - Vn;
				//pPtr->v = Vt;
				pPtr->v = pPtr->v - (vDir * N);
			}
			//Vector3 Vn = dotProduct( pPtr->v, N ) * N;
			//Vector3 Vt = pPtr->v - Vn;
			//pPtr->v = Vt;

			DP aDir =  dotProduct( pPtr->a, N );
			if( aDir < 1e-5 ){					//�� a ����V,���������� An
				pPtr->a = pPtr->a - ( aDir * N );
			}
			pPtr->a = pPtr->a - (1e-4f * pPtr->v);	// friction v==Vt
		}
	}
	//if( ccnt + tcnt > 2 ){
	//	printf("collide %d, contact %d\t", ccnt, tcnt );
	//}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// calculate particle.(a,v)
// �� strand particle ���۪� (a,v)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void StrandSystem::calcAccel( Strand & strand, DP timeStep )		
{
	testP2TriCollision_box_bvTree( strand, m_meshPtr ); // => test..._box_bvTree(...) �O�o�n strand.updateBox();
	
	//testP2TriCollision_bvTree( strand, m_meshPtr );
	addForceAccel( strand );		// �b addForecAccel() ���n�O�o a �k�s, a <= 0
	updateVA( strand );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// �[�@�ΤO/�[�t��
// particle.a += (���O,�Ů���O,�u®) (1/m)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StrandSystem::addForceAccel( Strand & strand )
{
	Gravity g;
	AirViscousity vi;

	Vector3 zero( 0, 0, 0 );
	for( vector<Particle>::iterator pPtr = strand.pList.begin() + 1;	// �Ĥ@�� particle ���δ�=>�H�bmesh�W
		pPtr != strand.pList.end(); ++pPtr )
	{
		//clear
		pPtr->a = zero;
		g.applyAccel( &(*pPtr) );
		vi.applyAccel( &(*pPtr) );
		m_wind.applyAccel( &( *pPtr ), m_time );
	}
	//�u®�@�ΤO
	for( vector<Spring>::iterator sPtr = strand.sList.begin();
		sPtr != strand.sList.end(); ++sPtr )
	{
		sPtr->applyAccel2particle( strand.pList );
	}
}


void StrandSystem::clearAccel( Strand & strand )
{
	Vector3 zero( 0, 0, 0 );
	for( vector<Particle>::iterator pPtr = strand.pList.begin();
		pPtr != strand.pList.end(); ++pPtr )
	{
		pPtr->a = zero;
	}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ODE integration
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void StrandSystem::Euler( Strand &strand, DP timeStep )
{
	clearAccel( strand );
	addForceAccel( strand );
	takeEulerStep( strand, strand, strand, timeStep );
}

//�]�w�Ȧs�� strand ���A
void StrandSystem::setupTempStrand( Strand &temp, const Strand &s )
{
	temp.pieceLen = s.pieceLen;
	temp.pList.resize( s.pList.size() );
	temp.sList.resize( s.sList.size() );
	temp.pTotal = s.pTotal;
	temp.sTotal = s.sTotal;
	//�] particle ����q/inverse Mass
	//�m�� STL �g�k
	vector<Particle>::iterator tptr = temp.pList.begin();
	vector<Particle>::const_iterator sptr = s.pList.begin();
	for( ; sptr != s.pList.end(); ++sptr, ++tptr ){
		tptr->invM = sptr->invM;
		tptr->status = sptr->status;
	}
	temp.sList.assign( s.sList.begin(), s.sList.end() );
	//for( unsigned j = 0; j < s.pList.size(); ++j ){
	//	temp.pList[j].invM = s.pList[j].invM;
	//	temp.pList[j].status = s.pList[j].status;
	//}
	//for( unsigned j = 0; j < s.sList.size(); ++j ){
	//	temp.sList[j] = s.sList[j];
	//}
}
void StrandSystem::midPoint( Strand &strand, DP h )
{
	// h is the size of a timeStep
	// y( t1 ) = y( t0 ) + h/2 * f(y(t0))
	// y(t + h) = y(t0)  + h* f(y(t1))

	// initialize temp strand
	copyStrand2strand( m_tempStrand[0], strand );

	clearAccel( strand );
	addForceAccel( strand );	// calc y'(0)=f( y(0) )
	// y( 0.5h ) = y( 0 ) + 0.5h* f( y(0) ) 
	takeEulerStep( m_tempStrand[0], strand, strand, 0.5f * h ); 

	clearAccel( m_tempStrand[0] );
	addForceAccel( m_tempStrand[0] );	// calc y'( 0.5h )=f( y(0.5h) )
	// y(h) = y(0) + h* f( y(0.5h) )
	takeEulerStep( strand, strand, m_tempStrand[0], h );
}


//�i�H������ copy constructor �a
void StrandSystem::copyStrand2strand( Strand &destination, const Strand &source )
{
	destination.pieceLen = source.pieceLen;
	destination.pList.resize( source.pList.size() );
	destination.sList.resize( source.sList.size() );
	destination.pTotal = source.pTotal;
	destination.sTotal = source.sTotal;
	destination.m_box = source.m_box;

	//�m�� STL ���g�k
	// assign range is [ first, last )
	destination.sList.assign( source.sList.begin(), source.sList.end() );
	destination.pList.assign( source.pList.begin(), source.pList.end() );
	//for( unsigned j = 0; j < source.pList.size(); ++j ){
	//	destination.pList[j] = source.pList[j];
	//}
	//for( unsigned j = 0; j < s.sList.size(); ++j ){
	//	destination.sList[j] = source.sList[j];
	//}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// perform weight sum 
//
// �� strand array A[] �� v, a ���v������, �v���b w[]
// ���G��b B
//
// B = ( w[1] ps[1] + w[2] ps[2] + .... + w[n] ps[n] ) / total( w[] )
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StrandSystem::weightedAverage( Strand &B, const Strand A[], int w[], int n )
{
	int total = 0;
	for( int y = 0; y < n; y++ ){
		total += w[y];
	}

	for( unsigned int i = 0; i < A[0].pList.size(); i++ ){

		for( int t = 0; t < 3; t++ ){

			DP temp = 0.0f, temp2 = 0.0f;
			for( int j = 0; j < n; j++ ){
				temp += w[j] * A[j][i].v[t];
				temp2 += w[j] * A[j][i].a[t];
			}
			B[i].v[t] = temp / total;
			B[i].a[t] = temp2 / total;
		}
	}
}// close function

void StrandSystem::RungeKutta( Strand &strand, DP h )
{
	// h is the size of a timeStep
	// y( t1 ) = y( 0 )							=> calc y'(t1)
	// y( t2 ) = y( 0 ) + 0.5h* f( y( t1 ) )    => calc y'(t2)
	// y( t3 ) = y( 0 ) + 0.5h* f( y( t2 ) )	=> calc y'(t3)
	// y( t4 ) = y( 0 ) + h * f( y(t3) )		=> calc y'(t4)
	// y( h ) = y( 0 ) + h* (1/6)( y'(t1) + 2* y'(t2) + 2* y'(t3) + y'(t4) )

	// initialize temp strand
	for( int i = 0; i < 5; i++ ){
		//copyStrand2strand( m_tempStrand[i], strand );
		m_tempStrand[i] = strand;
	}

	DP h2 = 0.5f * h;
	// y( t1 ) = y( 0 )
	calcAccel( m_tempStrand[0], h );	// calc f( y(t1) )

	// y( t2 ) = y( 0 ) + 0.5h * f( y(t1) ) 
	takeEulerStep( m_tempStrand[1], m_tempStrand[0], m_tempStrand[0], h2 ); 

	calcAccel( m_tempStrand[1], h2 );	// calc f( y(t2) )
	// y( t3 ) = y(0) + 0.5h * f( y(t2) )
	takeEulerStep( m_tempStrand[2], m_tempStrand[0], m_tempStrand[1], h2 ); 

	calcAccel( m_tempStrand[2], h2 );	// calc f( y(t3) )
	// y( t4 ) = y( 0 ) + h * f( y(t3) )
	takeEulerStep( m_tempStrand[3], m_tempStrand[0], m_tempStrand[2], h );

	calcAccel( m_tempStrand[3], h ); // calc  y'(t4)

	// calculate (1/6) ( y'(t1) + 2* y'(t2) + 2* y'(t3) + y'(t4) )

	int w[4] = { 1, 2, 2, 1 };
	weightedAverage( m_tempStrand[4], m_tempStrand, w, 4 );

	// result now in m_particles
	takeEulerStep( strand, m_tempStrand[0], m_tempStrand[4], h );
}
// y(h) = y(0) + h * f(y(t2))
// or
// y( t1 + h ) = y( t1 ) + h * f(y(t2) )
void StrandSystem::takeEulerStep( Strand &Y, const Strand &Y0, const Strand &Fy , DP h )
{
	vector<Particle>::iterator yptr;
	vector<Particle>::const_iterator y0ptr, fyptr;
	for( yptr = Y.pList.begin(), y0ptr = Y0.pList.begin(), fyptr = Fy.pList.begin(); yptr != Y.pList.end(); yptr++, y0ptr++, fyptr++ )
	{
		yptr->x = y0ptr->x + h * fyptr->v;
		yptr->v = y0ptr->v + h * fyptr->a;
		// p.s. �o�˼g target �i�H�O initial
		//�p�G�O
		//	v = ...
		//	x = ... �N���i�H, ���G�|���P�]�� v ���Ȥw�g�����ܤF
	}
}

void StrandSystem::setupTemp()
{
	int pTotal = m_strandList[0].pTotal;
	int sTotal = pTotal - 1;
	for( int i = 0; i < MAX_TEMP_NUM; ++i ){
		m_tempStrand[i].pList.resize( pTotal );
		m_tempStrand[i].sList.resize( sTotal );
	}

}
