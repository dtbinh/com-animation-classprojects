#include "MotionGraph.h"
#include "iostream"
#include "skeleton.h"
#include "fstream"
#include "transform.h"
#include "Math3D.h"

#include <stdlib.h>
#include <time.h>

#define NUM_BONES_IN_ASF_FILE 100

using namespace std;

float	m_jointWeights[NUM_BONES_IN_ASF_FILE];

float GetFacingAngle(Vector3 Ori)
{
	Matrix4 m = m.FromEulerAngle(Ori*deg2rad);
	return RADTODEG(atan2(m[8], m[10]));
}

double FrameDistance(Posture& p0, Posture& p1)
{
	double dist = 0;
	return dist;
}

MotionGraph::MotionGraph()
{
	offset = 0;
	m_NumMotions = 0;
	m_pPostures = NULL;
	m_LocalMinima.reserve(2000);
	m_Vertices = NULL;
}

MotionGraph::MotionGraph(char *amc_filename, float scale,Skeleton * pActor2)
{
	pActor = pActor2;

//	m_NumDOFs = actor.m_NumDOFs;
	offset = 0;
	m_NumFrames = 0;
	m_pPostures = NULL;
	readAMCfile(amc_filename, scale);
	m_EndFrames[0] = m_NumFrames-1;
	m_NumMotions = 1;
}

MotionGraph::~MotionGraph()
{
}

void MotionGraph::Concatenate(Motion& m1)
{
    Posture* new_Postures; 
    new_Postures = new Posture [m_NumFrames + m1.m_NumFrames];
 
    for(int i=0; i<m_NumFrames; i++)
	    new_Postures[i] = m_pPostures[i];

    for(int i=m_NumFrames ; i < m_NumFrames + m1.m_NumFrames; i++)
	    new_Postures[i] = m1.m_pPostures[i-m_NumFrames];

    delete [] m_pPostures; 

    m_NumFrames += m1.m_NumFrames;
    m_EndFrames[m_NumMotions] = m_NumFrames-1;
	m_NumMotions++;

    m_pPostures = new_Postures;
}
/*
 *	[DONE] Compute pose difference
 *	[DONE] Do pruning
 */
void MotionGraph::Construct()
{

	int i, j;
	double* pData;
	// Allocate pose difference matrix
	m_PoseDifference = (double**)malloc(m_NumFrames*sizeof(double*) + m_NumFrames*m_NumFrames*sizeof(double));
	for (i=0, pData = (double*)(m_PoseDifference+m_NumFrames); i < m_NumFrames; i++, pData+=m_NumFrames)
		m_PoseDifference[i] = pData;
	// Initialize pose difference matrix
	for (i=0; i<m_NumFrames; i++)
		for(j=0; j<m_NumFrames; j++)
			m_PoseDifference[i][j] = -1.0f;

	computePoseDifference();

	//	Find local minimum in search windows
	findLocalMinimum();

	//	Create graph structure
	createGraphStructure();

	//Do pruning
	findSCC();
}
/*
  *	Put the pose sequence into buffer when encounter transition.
  *	Parameter : "data" store pose vector that is pose sequence from previous transion to current transition.
  */
void MotionGraph::Transition(std::vector<Posture>& data)
{
	// for reference, you can replace with your code here

	if(buffer.empty())
	{
		for(int i=0;i<data.size();i++)
			buffer.push_back(data[i]);
	}
	else
	{
		Posture old_end;
		Posture new_start;
		Posture end;
		double angle;	//	angle difference between old_end and new_start

		old_end = buffer[buffer.size()-1];
		new_start = data[0];
		angle = GetFacingAngle(old_end.bone_rotation[0]) - GetFacingAngle(new_start.bone_rotation[0]);
		int len = data.size();
		if(len > TRANS_NUMS)
			len = TRANS_NUMS;
		end = data[len-1];
		if(len <= 2)
		{
			if(buffer.size() >= TRANS_NUMS)
			{
				Posture front;
				front = buffer[buffer.size()-TRANS_NUMS];
				new_start.Rotate(angle);
				for(int i=1;i<TRANS_NUMS;i++)
				{					
					// Blend front and new_start in different ratio as temp
					Posture temp = Slerp((float)i/(TRANS_NUMS-1), front, new_start);
					Posture source;
					source = buffer[buffer.size()-TRANS_NUMS+i];
					temp.root_pos.x = source.root_pos.x;
					temp.root_pos.z = source.root_pos.z;
					// Blend poses in buffer with temp
					buffer[buffer.size()-TRANS_NUMS+i] = Slerp((float)i/(TRANS_NUMS-1), source, temp);
				}
			}
			else
			{
				// interpolation
				new_start.Rotate(angle);
				for(int k=1;k<TRANS_NUMS-1;k++)
				{
					Posture temp;
					//	Blend old_end and new_start in different ratio as temp
					temp = Slerp((float)k/(len-1), old_end, new_start);
					temp.root_pos.x = old_end.root_pos.x;
					temp.root_pos.z = old_end.root_pos.z;
					buffer.push_back(temp);
				}
			}
			len = 0;
		}

		end.Rotate(angle);
		for(int k=1;k<data.size();k++)
		{				
			Posture p;				
			if(k < len)
			{
				Posture temp;
				temp = Slerp((float)k/(len-1), old_end,end);
				Posture source;
				source = data[k];
				source.Rotate(angle);
				
				temp.root_pos.x = source.root_pos.x;
				temp.root_pos.z = source.root_pos.z;
				
				p = Slerp((float)k/(len-1), temp, source);
				p.bone_rotation[0] = source.bone_rotation[0];
			}
			else
			{
				p = data[k];
				p.Rotate(angle);
			}

			Vector3 disp = p.root_pos - new_start.root_pos;
			Matrix4 RyTrans = Matrix4::Yrotate(angle * deg2rad);
			disp = RyTrans * disp;
			p.root_pos.x = old_end.root_pos.x + disp.x;
			p.root_pos.z = old_end.root_pos.z + disp.z;
			
			buffer.push_back(p);			
		}
	}
}

void MotionGraph::Transition(Posture& pose)
{

}

int	MotionGraph::NextJump(int index)
{
	int i, adjSize, next = -1;
	int defaultFrame;
	MotionVertex *curVertex, *adjVertex, *nextFrameVertex;
	double	accProb = 0.0f, Prob;

	//srand(time(NULL));
	Prob = double(rand() % 1000) / 1000.0;

	if (Prob < 0.0 || Prob > 1.0)
	{
		cout << "Prob error!" << endl;
		exit(1);
	}

	curVertex =  &m_Vertices[index];
	adjSize = curVertex->m_AdjVertices.size();

	for (i = 0; i < adjSize; i++)
	{
		adjVertex = curVertex->m_AdjVertices[i];
		if (adjVertex->m_InSCC)
		{

			//	If only 1 adjVertex in SCC, return it
			if (curVertex->m_NumSCCAdj == 1)
				return adjVertex->m_FrameIndex;

			if (curVertex->m_NextFrameInSCC)	//	next frame is in SCC
			{
				if (adjVertex->m_FrameIndex == (curVertex->m_MotionIndex + 1))
					accProb += 0.5f;
				else
					accProb += (0.5/(double)(curVertex->m_NumSCCAdj - 1));
			}
			else	//	next frame is not in SCC, each adjVertex has the same chance
			{
				accProb += (1.0/(double)curVertex->m_NumSCCAdj);
			}
			/*
			if (accProb > 1.0f)
			{
				cout << "accProb error:accProb=" << accProb << endl;
				system("PAUSE");
			}*/
			if (accProb >= Prob)
			{	
				next = adjVertex->m_FrameIndex;
				return next;
			}
			defaultFrame = adjVertex->m_FrameIndex;
		}
	}

	return defaultFrame;
}

//	Traverse motion graph
int MotionGraph::Traverse(int current, bool& jump)
{
	jump = false;
	int next = 0;
/*
	vector<Posture> v;
	v.push_back(m_pPostures[current]);
	if(buffer.size() < 1000)
	{
		//Transition(v);
		buffer.push_back(m_pPostures[current]);
		
	}*/
	next = NextJump(current);
	if (next == -1)
	{
		cout << "NextJump error" << endl;
		exit(1);
	}
	

	return next;
}

int MotionGraph::GenerateMotion(int total_frames, int start_frame, char* filename)
{
	Motion outMotion(total_frames, pActor);

	int next = start_frame;
	while(buffer.size() < total_frames)
		next = NextJump(next);
	
	for(int i=0;i<total_frames;i++)
		outMotion.SetPosture(i, buffer[i]);
	
	outMotion.writeAMCfile(filename, MOCAP_SCALE);
	return next;
}

void MotionGraph::printJumpIdx(int current, int next )
{
	int i, cur_clipIdx, cur_frameIdx, next_clipIdx, next_frameIdx;
	for(i=0; i<m_NumMotions; i++)
	{
      if( m_EndFrames[i] >= current )
		  break; 
	}

   cur_clipIdx = i;
   if(i > 0)
	   cur_frameIdx = current - m_EndFrames[i-1];
   else
	   cur_frameIdx = current;

	for(i=0; i<m_NumMotions; i++)
	{	
		if( m_EndFrames[i] >= next )
			break; 
	}

   next_clipIdx = i;
   if(i>0)
	   next_frameIdx = next-m_EndFrames[i-1];
   else
	   next_frameIdx = next;

   //printf("jump from ( %d, %d ) to %d, %d\n", frame no, cur_clipIdx, cur_frameIdx, next_clipIdx, next_frameIdx);
   printf("jump from ( %d, %d ) to ( %d, %d )\n", cur_clipIdx, cur_frameIdx, next_clipIdx, next_frameIdx);
}

void MotionGraph::setActor(Skeleton *pActor)
{
	this->pActor = pActor;
}

/*
 *	Define pose difference as Dij  = d(pi, pj ) + £hd(vi, vj ).
 *	d(pi, pj ) : weighted differences of joint angles,
 *	d(vi, vj ) : weighted differences of joint velocities
 *	[TODO] Preserve dynamics
 */
void MotionGraph::computePoseDifference()
{
	int i, j;
	Posture *p1, *p2;
	double tmp;
	
	//	Compute joint velocities
	
	for (i=0, j=0; i<m_NumFrames; i++)
	{
		if (i != m_EndFrames[j])
		{
			m_pPostures[i].computeJointVelocities(m_pPostures[i+1], true); 
		}
		else
		{
			m_pPostures[i].computeJointVelocities(m_pPostures[i-1], false); 
			j++;
		}
	}


	for (i=0; i < m_NumFrames; i++)
	{
		p1 = &m_pPostures[i];
		for (j=0; j < m_NumFrames; j++)
		{
			p2 = &m_pPostures[j];
			
			
			if (m_PoseDifference[j][i] >= 0.0f)
				m_PoseDifference[i][j] = m_PoseDifference[j][i];
			else
			{
				//	Diff of joint orientation
				m_PoseDifference[i][j] = Posture::compareJointAngles(*p1, *p2);
				//	Diff of joint velocity
				m_PoseDifference[i][j] += (double)VELOCITY_WEIGHT * Posture::compareJointVelocities(*p1, *p2);
			}
			//cout << "dist[" << i << "][" << j << "]=" << m_PoseDifference[i][j] << endl;
			
			
				
		}
		cout << "i=" << i << endl;
	}

}

/*
 *	Find local minimum of each window in the pose difference matrix
 */
void MotionGraph::findLocalMinimum()
{
	int i, j, p, q;
	const int winSize = 50;
	int rowBound, colBound;
	double localMin;
	int minI, minJ;
	/*
	// For statisitics
	int cnt = 0;
	double min = 9999.0f, max = 0.0f, sum = 0.0f;
*/
	for (i = 0; i < m_NumFrames; i += winSize)
		for (j = 0; j < m_NumFrames; j += winSize)
		{
			if (i+winSize < m_NumFrames)
				rowBound = i+winSize;
			else
				rowBound = m_NumFrames;

			if (j+winSize < m_NumFrames)
				colBound = j+winSize;
			else
				colBound = m_NumFrames;
			//	Search inside a window
			localMin = 9999.0f;
			for (p = i; p < rowBound; p++)
				for (q = j; q < colBound; q++)
				{
					if (p == q)
						continue;
					if (m_PoseDifference[p][q] < localMin)
					{
						localMin = m_PoseDifference[p][q];
						minI = p;
						minJ = q;

					}
				}
			if (localMin < Threshold)
			{
				m_LocalMinima.push_back(pair<int, int>(minI, minJ));
			}
			/*
			//	Gather statistics
			if (localMin < min)
				min = localMin;
			if (localMin > max)
				max = localMin;
			sum += localMin;
			cnt++;
			*/
		}
/*
	cout << "min = " << min << endl;
	cout << "max = " << max << endl;
	cout << "avg = " << sum / (double) cnt << endl;
	*/
}

void MotionGraph::createGraphStructure()
{
	int i, j;
	MotionVertex*	pVertex;

	//	Allocate motion vertices
	m_Vertices = new MotionVertex[m_NumFrames];
	pVertex = m_Vertices;
	for (i = 0, j = 0; i < m_NumFrames; i++)
	{
		pVertex = &m_Vertices[i];
		pVertex->m_MotionIndex = j;
		pVertex->m_FrameIndex = i;
		//	Connect edges of vertex in same motion
		if (i != m_EndFrames[j])
			pVertex->m_AdjVertices.push_back(&m_Vertices[i+1]);
		else
		{
			j++;
		}
	}
/*
	for (i = 0, j = 0; i < m_NumFrames; i++)
	{
		pVertex = &m_Vertices[i];
		if (i != m_EndFrames[j])
			if (pVertex->m_AdjVertices.size() == 0)
			{
				cout << "adj Exception : i=" << i << endl;
				system("PAUSE");
			}
		else
			j++;
	}
*/
	//	Connect edges for local minima
	int size = (int) m_LocalMinima.size();
	std::pair<int, int> *pLocalMin;
	for (i = 0; i < size; i++)
	{
		pLocalMin = &m_LocalMinima[i];
		m_Vertices[pLocalMin->first].m_AdjVertices.push_back(&m_Vertices[pLocalMin->second]);

	}
}

void MotionGraph::DFS(bool SCCOrder)
{
	int i;
	int time = 0;
	MotionVertex*	pVertex;

	if (m_Vertices == NULL)
	{
		cout << "Exception in DFS():m_Vertices is null" << endl;
		exit(1);
	}

	for (i=0; i<m_NumFrames; i++)
	{
		pVertex = &m_Vertices[i];
		pVertex->m_Color = MotionVertex::WHITE;
		pVertex->m_Pi = NULL;
	}

	if (!SCCOrder)
		for (i=0; i<m_NumFrames; i++)
		{
			pVertex = &m_Vertices[i];
			if(pVertex->m_Color == MotionVertex::WHITE)
			{
				DFS_Visit(pVertex, &time, false);
			}
		}
	else
		for (i=(int)m_SCCQueue.size()-1; i>=0; i--)
		{
			pVertex = &m_Vertices[i];
			if(pVertex->m_Color == MotionVertex::WHITE)
			{
				DFS_Visit(pVertex, &time, true);
			}
		}

}

void MotionGraph::DFS_Visit(MotionVertex* u, int* time, bool SCCOrder)
{
	int i, size;
	MotionVertex* v;

	u->m_Color = MotionVertex::GRAY;
	(*time) = (*time) + 1;
	u->m_DTime = *time;
if (!SCCOrder)	//	Traverse G
{
	size = u->m_AdjVertices.size();
	for (i = 0; i < size; i++)
	{
		v = u->m_AdjVertices[i];
		if (v->m_Color == MotionVertex::WHITE)
		{
			v->m_Pi =  u;
			DFS_Visit(v, time, SCCOrder);
		}
	}
}
else	//	Traverse G^T
{
	size = u->m_TransposeAdjVertices.size();
	for (i = 0; i < size; i++)
	{
		v = u->m_TransposeAdjVertices[i];
		if (v->m_Color == MotionVertex::WHITE)
		{
			v->m_Pi =  u;
			DFS_Visit(v, time, SCCOrder);
		}
	}
}

	u->m_Color = MotionVertex::BLACK;
	(*time) = (*time) + 1;
	u->m_FTime = *time;

	if (!SCCOrder)
		m_SCCQueue.push_back(u);

}

void MotionGraph::findSCC()
{
	int i, j, size;
	MotionVertex* v;
	int order;
	int maxInterval = 0;

	DFS(false);
	// compute transpose graph
	for	(i = 0; i < m_NumFrames; i++)
	{
		v = &m_Vertices[i];
		size = v->m_AdjVertices.size();
		for	(j = 0; j < size; j++)
		{
			v->m_AdjVertices[j]->m_TransposeAdjVertices.push_back(v);
		}
	}

	//	Do DFS on transpose graph in decreasing FTime order
	DFS(true);

	// Find the largest SCC
	for (i=0; i<m_NumFrames; i++)
	{
		v = &m_Vertices[i];
		if (v->m_Pi == NULL)
			if ((v->m_FTime - v->m_DTime) > maxInterval)
			{
				maxInterval = (v->m_FTime - v->m_DTime);
				m_MaxSCCRoot = v;
			}
	}
//	Identify all vertices in SCC
	for (i=0; i<m_NumFrames; i++)
	{
		v = &m_Vertices[i];
		if ( (v->m_DTime >= m_MaxSCCRoot->m_DTime) &&
			  (v->m_FTime <= m_MaxSCCRoot->m_FTime) )
		{
			if (v->m_AdjVertices.size() > 0)
				v->m_InSCC = true;	
		}
	}

	for (i=0; i<m_NumFrames; i++)
	{
		v = &m_Vertices[i];
		if (v->m_InSCC)
		{
			v->computeNumSCCAdj();
			if ( ((v->m_FrameIndex + 1) < m_NumFrames) &&
				(m_Vertices[v->m_FrameIndex + 1].m_MotionIndex == v->m_MotionIndex) &&
				(m_Vertices[v->m_FrameIndex + 1].m_InSCC) )
			{
				v->m_NextFrameInSCC = true;
			}
		}
	}
}