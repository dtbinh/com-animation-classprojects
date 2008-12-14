#include "MotionGraph.h"
#include "iostream"
#include "skeleton.h"
#include "fstream"
#include "transform.h"
#include "Math3D.h"

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
 *	[TODO] Compute pose difference
 *	[TODO] Do pruning
 */
void MotionGraph::Construct()
{

	int i;
	double* pData;
	// Allocate pose difference matrix
	m_PoseDifference = (double**)malloc(m_NumFrames*sizeof(double*) + m_NumFrames*m_NumFrames*sizeof(double));
	for (i=0, pData = (double*)(m_PoseDifference+m_NumFrames); i < m_NumFrames; i++, pData+=m_NumFrames)
		m_PoseDifference[i] = pData;

	computePoseDifference();
}

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
		old_end = buffer[buffer.size()-1];
		Posture new_start;
		new_start = data[0];
		double angle = GetFacingAngle(old_end.bone_rotation[0]) - GetFacingAngle(new_start.bone_rotation[0]);
		int len = data.size();
		if(len > TRANS_NUMS)
			len = TRANS_NUMS;
		Posture end;
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
					Posture temp = Slerp((float)i/(TRANS_NUMS-1), front, new_start);
					Posture source;
					source = buffer[buffer.size()-TRANS_NUMS+i];
					temp.root_pos.x = source.root_pos.x;
					temp.root_pos.z = source.root_pos.z;
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

int	MotionGraph::NextJump(int index)
{
	// Add your code here
	return 0;
}

int MotionGraph::Traverse(int current, bool& jump)
{
	jump = false;
	int next = 0;

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
 */
void MotionGraph::computePoseDifference()
{
	int i, j;
	Posture *p1, *p2;
	
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
			//	Diff of joint orientation
			m_PoseDifference[i][j] = Posture::compareJointAngles(*p1, *p2);
			//cout << "dist[" << i << "][" << j << "]=" << m_PoseDifference[i][j] << endl;
			
			//	Diff of joint velocity
				
		}
		cout << "i=" << i << endl;
	}

}