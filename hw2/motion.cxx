#include <stdio.h>
#include <string.h>
#include <fstream>
#include <math.h>

#include "skeleton.h"
#include "motion.h"
#include "vector.h"

// a default skeleton that defines each bone's degree of freedom and the order of the data stored in the AMC file
//static Skeleton actor("Skeleton.ASF", MOCAP_SCALE);
typedef float * floatptr;

using namespace std;


/************************ Motion class functions **********************************/
Motion::Motion(int nNumFrames)
{
//	m_NumDOFs = pActor.m_NumDOFs;
	
	m_NumFrames = nNumFrames;
	offset = 0;

	//allocate postures array
	m_pPostures = new Posture [m_NumFrames];

	//Set all postures to default posture
	SetPosturesToDefault();
}

Motion::Motion(int nNumFrames, Skeleton * pActor2)
{
	pActor = pActor2;
	//	m_NumDOFs = pActor.m_NumDOFs;
	
	m_NumFrames = nNumFrames;
	offset = 0;

	//allocate postures array
	m_pPostures = new Posture [m_NumFrames];

	//Set all postures to default posture
	SetPosturesToDefault();
}

Motion::Motion(char *amc_filename, float scale,Skeleton * pActor2)
{
	pActor = pActor2;

//	m_NumDOFs = actor.m_NumDOFs;
	offset = 0;
	m_NumFrames = 0;
	m_pPostures = NULL;
	readAMCfile(amc_filename, scale);	
}

Motion::Motion(char *amc_filename, float scale)
{
//	m_NumDOFs = actor.m_NumDOFs;
	offset = 0;
	m_NumFrames = 0;
	m_pPostures = NULL;
	readAMCfile(amc_filename, scale);
}


Motion::~Motion()
{
	if (m_pPostures != NULL)
		delete [] m_pPostures;
}


//Set all postures to default posture
void Motion::SetPosturesToDefault()
{
	//for each frame
	//int numbones = numBonesInSkel(bone[0]);
	for (int i = 0; i<MAX_BONES_IN_ASF_FILE; i++)
	{
		//set root position to (0,0,0)
		m_pPostures[i].root_pos.set(0.0, 0.0, 0.0);
		//set each bone orientation to (0,0,0)
		for (int j = 0; j < MAX_BONES_IN_ASF_FILE; j++)
			m_pPostures[i].bone_rotation[j].set(0.0, 0.0, 0.0);

	}
}

//Set posture at spesified frame
void Motion::SetPosture(int nFrameNum, Posture InPosture)
{
	m_pPostures[nFrameNum] = InPosture; 	
}

int Motion::GetPostureNum(int nFrameNum)
{
	nFrameNum += offset;

	if (nFrameNum < 0)
		return 0;
	else if (nFrameNum >= m_NumFrames)
		return m_NumFrames-1;
	else
		return nFrameNum;
	return 0;
}

void Motion::SetTimeOffset(int n_offset)
{
	offset = n_offset;
}

void Motion::SetBoneRotation(int nFrameNum, Vector3 vRot, int nBone)
{
	m_pPostures[nFrameNum].bone_rotation[nBone] = vRot;
}

void Motion::SetRootPos(int nFrameNum, Vector3 vPos)
{
	m_pPostures[nFrameNum].root_pos = vPos;
}


Posture* Motion::GetPosture(int nFrameNum)
{
	if (m_pPostures != NULL) 
		return &m_pPostures[nFrameNum]; 
	else 
		return NULL;
}


int Motion::readAMCfile(char* name, float scale)
{
	Bone *hroot, *bone;
	bone = hroot= (*pActor).getRoot();

	ifstream file( name, ios::in);
	if( !file ) 
	{
	   return -1;
	}
	else
	{
	   file.close();
       file.open(name,ios::out);
	}

	int n=0;
	char str[2048];

	// count the number of lines
	while(!file.eof())  
	{
		file.getline(str, 2048);
		if(file.eof()) break;
		//We do not want to count empty lines
		if (strcmp(str, "") != 0)
			n++;
	}

	file.close();
	file.clear();

	//Compute number of frames. 
	//Subtract 3 to  ignore the header
	//There are (NUM_BONES_IN_ASF_FILE - 2) moving bones and 2 dummy bones (lhipjoint and rhipjoint)
	int numbones = numBonesInSkel(bone[0]);
	int movbones = movBonesInSkel(bone[0]);
	n = (n-3)/((movbones) + 1);   

	m_NumFrames = n;

	//Allocate memory for state vector
	m_pPostures = new Posture [m_NumFrames]; 

	file.open( name );

	// skip the header
	while (1) 
	{
		file >> str;
		if(strcmp(str, ":DEGREES") == 0) break;
	}

	int frame_num;
	float x, y, z;
	int i, bone_idx, state_idx;

	for(i=0; i<m_NumFrames; i++)
	{
		//read frame number
		file >> frame_num;
		x=y=z=0;

		//There are (NUM_BONES_IN_ASF_FILE - 2) moving bones and 2 dummy bones (lhipjoint and rhipjoint)
		for( int j=0; j<movbones; j++ )
		{
			//read bone name
			file >> str;
			
			//Convert to corresponding integer
			for( bone_idx = 0; bone_idx < numbones; bone_idx++ )
//				if( strcmp( str, AsfPartName[bone_idx] ) == 0 ) 
				if( strcmp( str, pActor->idx2name(bone_idx) ) == 0 ) 

					break;


			//init rotation angles for this bone to (0, 0, 0)
			m_pPostures[i].bone_rotation[bone_idx].set(0.0, 0.0, 0.0);

			for(int x = 0; x < bone[bone_idx].dof; x++)
			{
				float tmp;
				file >> tmp;
			//	printf("%d %f\n",bone[bone_idx].dofo[x],tmp);
				switch (bone[bone_idx].dofo[x]) 
				{
					case 0:
						printf("FATAL ERROR in bone %d not found %d\n",bone_idx,x);
						x = bone[bone_idx].dof;
						break;
					case 1:
						m_pPostures[i].bone_rotation[bone_idx].x = tmp;
						break;
					case 2:
						m_pPostures[i].bone_rotation[bone_idx].y = tmp;
						break;
					case 3:
						m_pPostures[i].bone_rotation[bone_idx].z = tmp;
						break;
					case 4:
						m_pPostures[i].bone_translation[bone_idx].x = tmp * scale;
						break;
					case 5:
						m_pPostures[i].bone_translation[bone_idx].y = tmp * scale;
						break;
					case 6:
						m_pPostures[i].bone_translation[bone_idx].z = tmp * scale;
						break;
					case 7:
						m_pPostures[i].bone_length[bone_idx].x = tmp;// * scale;
						break;
				}
			}
			if( strcmp( str, "root" ) == 0 ) 
			{
				m_pPostures[i].root_pos.x = m_pPostures[i].bone_translation[0].x;// * scale;
				m_pPostures[i].root_pos.y = m_pPostures[i].bone_translation[0].y;// * scale;
				m_pPostures[i].root_pos.z = m_pPostures[i].bone_translation[0].z;// * scale;
			}


			// read joint angles, including root orientation
			
		}
	}

	file.close();
	printf("%d samples in '%s' are read.\n", n, name);
	return n;
}

int Motion::writeAMCfile(char *filename, float scale)
{
	int f, n, j, d;
	Bone *bone;
	bone=(*pActor).getRoot();

	ofstream os(filename);
	if(os.fail()) return -1;


	// header lines
	os << "#Unknow ASF file" << endl;
	os << ":FULLY-SPECIFIED" << endl;
	os << ":DEGREES" << endl;
	int numbones = numBonesInSkel(bone[0]);

	for(f=0; f < m_NumFrames; f++)
	{
		os << f+1 <<endl;
		os << "root " << m_pPostures[f].root_pos.x/scale << " " 
			          << m_pPostures[f].root_pos.y/scale << " " 
					  << m_pPostures[f].root_pos.z/scale << " " 
					  << m_pPostures[f].bone_rotation[root].x << " " 
					  << m_pPostures[f].bone_rotation[root].y << " " 
					  << m_pPostures[f].bone_rotation[root].z ;
		n=6;
		
		for(j = 2; j < numbones; j++) 
		{

			//output bone name
			if(bone[j].dof != 0)
//				os << endl << AsfPartName[j];
				os << endl << pActor->idx2name(j);

			//output bone rotation angles
			if(bone[j].dofx == 1) 
				os << " " << m_pPostures[f].bone_rotation[j].x;

			if(bone[j].dofy == 1) 
				os << " " << m_pPostures[f].bone_rotation[j].y;

			if(bone[j].dofz == 1) 
				os << " " << m_pPostures[f].bone_rotation[j].z;
		}
		os << endl;
	}

	os.close();
	printf("Write %d samples to '%s' \n", m_NumFrames, filename);
	return 0;
}



