#ifndef	MOTIONVERTEX_H
#define	MOTIONVERTEX_H

#include <vector>

using namespace std;

class MotionVertex
{
public:
	//----------------Member functions
	MotionVertex();

public:
	//----------------Static variables
	const static int WHITE = 0;
	const static int BLACK = 1;
	const static int GRAY  = 2;

	const static int NIL   = -1;	// No predecessor

	//----------------Member variables
	vector<int>	m_AdjVertices;	//	Adjacent vertices
	int	m_MotionIndex;			//	Pose of this MotionVertex belongs to which motion
	int	m_FrameIndex;			//	Frame index of this MotionVertex in MotionGraph
		// Variables for DFS
	int m_Color;
	int m_Pi;					//	Predecessor
	int m_DTime;				//	Discovery time
	int m_FTime;				//	Finish time
	
};

#endif