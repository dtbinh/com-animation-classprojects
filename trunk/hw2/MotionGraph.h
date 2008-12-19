#ifndef MOTIONGRAPH_H
#define MOTIONGRAPH_H

#include "Posture.h"
#include "Motion.h"
#include "MotionVertex.h"
#include <vector>

#define Threshold	0.05f//0.03f
#define MAX_DIST	100
#define MAX_MOTION	100
#define Window		30
#define Motion_NUMS	23
#define TRANS_NUMS 5

using namespace std;
/*
enum AsfPart	
{
	Root=0,
	lhipjoint, lfemur, ltibia, lfoot, ltoes,
	rhipjoint, rfemur, rtibia, rfoot, rtoes,
	lowerback, upperback, thorax, lowerneck, upperneck, head,
	lclavicle, lhumerus, lradius, lwrist, lhand, lfingers, lthumb,
	rclavicle, rhumerus, rradius, rwrist, rhand, rfingers, rthumb
};
*/
class MotionGraph : public Motion
{
public:
			MotionGraph();
			MotionGraph(char *amc_filename, float scale,Skeleton * pActor);
			~MotionGraph();
	void	Concatenate(Motion& m1);
	void	Construct();
	int		Traverse(int current, bool& jump);
	int		Traverse1(int current, bool& jump);
	int		GenerateMotion(int total_frames, int start_frame, char*);
	void	Transition(std::vector<Posture>& data);
	void	Transition1(std::vector<Posture>& data);
	int		NextJump(int index);
	void	setActor(Skeleton *pActor);
	void	computePoseDifference();
	void	findLocalMinimum();
	void	createGraphStructure();
	void	DFS(bool SCCOrder);										//	Depth-first search
	void	DFS_Visit(MotionVertex* u, int* time, bool SCCOrder);	//	Do DFS visit for vertex v
	void	findSCC();												//	Find strongly connected components
private:
	void	printJumpIdx(int current, int next);

public:
	const static int VELOCITY_WEIGHT = 5;
	int		m_NumMotions;
	int		m_EndFrames[MAX_MOTION]; // max number of segments can be added in a motion.
	std::vector<Posture> buffer;
	double**					m_PoseDifference;	// matrix of pose difference
	vector<std::pair<int, int>> m_LocalMinima;		//	Local minima
	MotionVertex*				m_Vertices;
	vector<MotionVertex*>		m_SCCQueue;			//	Vertex order for SCC computation	
	MotionVertex*						m_MaxSCCRoot;
	int	m_BufferIndex;
};

#endif