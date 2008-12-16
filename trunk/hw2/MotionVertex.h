#ifndef	MOTIONVERTEX_H
#define	MOTIONVERTEX_H

#include <vector>

using namespace std;

class MotionVertex
{
public:
	//----------------Member functions
	MotionVertex() {}

	//----------------Member variables
	vector<int>	m_AdjVertices;	//	Adjacent vertices
	int	m_MotionIndex;				//	Pose of this MotionVertex belongs to which motion
	int	m_FrameIndex;					//	Frame index of this MotionVertex in MotionGraph
	
};

#endif