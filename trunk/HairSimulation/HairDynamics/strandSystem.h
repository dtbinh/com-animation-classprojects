#ifndef STRAND_SYSTEM_H
#define STRAND_SYSTEM_H

#include	"line.h"
#include	"particle.h"
//#include	"strand.h"
#include	"mesh.h"


/****************************************************************************/
/*   StrandSystem: many strands                                             */
/****************************************************************************/
#define MAX_TEMP_NUM 6
#define EULER		0
#define MID_POINT	1
#define RUNGE_KUTTA	2

class StrandSystem{
public:
	std::vector<Strand> m_strandList;	// strand list
	int m_totalStrand;
	
	DP m_time;
	

	void setup( vector<Line> &lineSet, mesh *meshPtr );
	//make many strands from a lineSet
	void setupStrand( std::vector<Line> &lineSet );
	void drawStrandBoxes();				// draw strand's bounding box

	void setMeshObject( mesh *meshPtr )	// mesh object in the scene
	{
		assert( meshPtr != 0 );
		m_meshPtr = meshPtr;
	}

	//subscript operator for non-const objects returns modifiable lvalue
	Strand& operator[](int index)
	{
		return m_strandList[index];	//reference
	}
	Strand operator[]( int index ) const
	{
		return m_strandList[index];	//copy of this element
	}

	void step( DP timeStep );
	void setStepMethod( int m );


private:
	mesh *m_meshPtr;	// a mesh object		//std::vector< mesh *> m_meshList;	// objects in the scene
	Wind m_wind;		// wind
	//DP m_ballR2;		// squared Radius of ball
	//
	//void updateStatus( Strand & strand );
	void calcAccel( Strand & strand, DP h );		// calculate particle.(a,v)
	void addForceAccel( Strand & strand );	// particle.a += (重力,空氣阻力,彈簧) (1/m)
	void clearAccel( Strand & strand );		// particle.a<-0

	void updateStrandBoxes();				// update strand bounding boxes
	// test collision event
	void testP2TriCollision_box_bvTree( Strand & strand, mesh *meshPtr );
	void testP2TriCollision( Strand & strand, mesh *meshPtr );
	void testP2TriCollision_bvTree( Strand & strand, mesh *meshPtr );

	//ODE integration
	void setupTempStrand( Strand &temp, const Strand &s );
	void Euler( Strand &strand, DP timeStep );
	void midPoint( Strand &strand, DP h );
	void copyStrand2strand( Strand &destination, const Strand &source );
	void weightedAverage( Strand &B, const Strand A[], int w[], int n );
	void RungeKutta( Strand &strand, DP h );
	// y(h) = y(0) + h * f(y(t2))
	// or
	// y( t1 + h ) = y( t1 ) + h * f(y(t2) )
	void takeEulerStep( Strand &Y, const Strand &Y0, const Strand &Fy, DP h );

	Strand m_tempStrand[ MAX_TEMP_NUM ];
	int m_stepMethod;
	void setupTemp();

};
#endif