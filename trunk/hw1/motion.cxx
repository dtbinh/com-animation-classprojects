#include <stdio.h>
#include <string.h>
#include <iostream>
#include <math.h>

#include "skeleton.h"
#include "motion.h"
#include "vector.h"
#include <iostream>
#include <fstream>

using namespace std;

// a default skeleton that defines each bone's degree of freedom and the order of the data stored in the AMC file
static Skeleton actor("Skeleton.ASF", MOCAP_SCALE);
typedef float * floatptr;


Motion::Motion(char *amc_filename, float scale)
{
	m_NumDOFs = actor.m_NumDOFs;
	readAMCfile(amc_filename, scale);
}


Motion::~Motion()
{
	delete [] m_pPostures;
}


int Motion::readAMCfile(char* name, float scale)
{
	Bone *hroot, *bone;
	bone=hroot=actor.getRoot();

	
	//std::ifstream file( name, ios::in | ios::nocreate );
    ifstream file( name, ios::in);

	int n=0;
	char str[2048];
        
        if( file.fail() ) return -1;
	
        // count the number of lines
	while(!file.eof())  
	{
		file.getline(str, 2048);
		if(file.eof()) break;
		if(strcmp(str, "") !=0) n++;
	}

	file.close();
	file.clear();
        

	//Compute number of frames. 
	//Subtract 3 to  ignore the header
	//There are (NUM_BONES_IN_ASF_FILE - 2) moving bones and 2 dummy bones (lhipjoint and rhipjoint)
	n = (n-3)/((NUM_BONES_IN_ASF_FILE - 2) + 1);   

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
		for( int j=0; j<NUM_BONES_IN_ASF_FILE-2; j++ )
		{
			//read bone name
			file >> str;
			
			//Convert to corresponding integer
			for( bone_idx = 0; bone_idx < NUM_BONES_IN_ASF_FILE; bone_idx++ )
				if( strcmp( str, AsfPartName[bone_idx] ) == 0 ) 
					break;

			//If root -> read root position
			if( strcmp( str, "root" ) == 0 ) 
			{
				file >> x >> y >> z;
				m_pPostures[i].root_pos.p[0] = x * scale;
				m_pPostures[i].root_pos.p[1] = y * scale;
				m_pPostures[i].root_pos.p[2] = z * scale;
			}

			//init rotation angles for this bone to (0, 0, 0)
			m_pPostures[i].bone_rotation[bone_idx].setValue(0.0, 0.0, 0.0);

			// read joint angles, including root orientation
			if(bone[bone_idx].dofx == 1) 
				file >> m_pPostures[i].bone_rotation[bone_idx].p[0];

			if(bone[bone_idx].dofy == 1) 
				file >> m_pPostures[i].bone_rotation[bone_idx].p[1];

			if(bone[bone_idx].dofz == 1) 
				file >> m_pPostures[i].bone_rotation[bone_idx].p[2];
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
	bone=actor.getRoot();

	ofstream os(filename);
	if(os.fail()) return -1;


	// header lines
	os << "#Unknow ASF file" << endl;
	os << ":FULLY-SPECIFIED" << endl;
	os << ":DEGREES" << endl;

	for(f=0; f < m_NumFrames; f++)
	{
		os << f+1 <<endl;
		os << "root " << m_pPostures[f].root_pos.p[0]/scale << " " 
			          << m_pPostures[f].root_pos.p[1]/scale << " " 
					  << m_pPostures[f].root_pos.p[2]/scale << " " 
					  << m_pPostures[f].bone_rotation[root].p[0] << " " 
					  << m_pPostures[f].bone_rotation[root].p[1] << " " 
					  << m_pPostures[f].bone_rotation[root].p[2] ;
		n=6;
		for(j = 2; j < NUM_BONES_IN_ASF_FILE; j++) 
		{

			//output bone name
			if(bone[j].dof != 0)
				os << endl << AsfPartName[j];

			//output bone rotation angles
			if(bone[j].dofx == 1) 
				os << " " << m_pPostures[f].bone_rotation[j].p[0];

			if(bone[j].dofy == 1) 
				os << " " << m_pPostures[f].bone_rotation[j].p[1];

			if(bone[j].dofz == 1) 
				os << " " << m_pPostures[f].bone_rotation[j].p[2];
		}
		os << endl;
	}

	os.close();
	printf("Write %d samples to '%s' \n", m_NumFrames, filename);
	return 0;
}


// Save the motion data in Mrdplot format. It includes a header and data.
// The header specifies the dimension of the data, and the variable names and units associated with the data. 
// The data are just a collection of state vectors at different frames. 
int Motion::writeMRDfile(char *filename)
{

//DEBUG: Important, fix this function 
#if 0
	int i;
	FILE *fp;
   
	fp=fopen(filename,"wt");
	if(fp==NULL) return 0;

	// the first line specifies the total length, dimension, total sample of data, frequency  
	fprintf(fp, "%d %d %d 1.00", m_NumFrames*m_NumDOFs, m_NumDOFs, m_NumFrames);

	// write variable names and units
	for(i=0;i<3;i++)
		fprintf(fp, "%s  m  ", StateIdx[i]);

	for(i=3;i<m_NumDOFs;i++)
		fprintf(fp, "%s  deg  ", StateIdx[i]);

	fprintf(fp, "\n");

	fwrite(&m_State[0][0], sizeof(float), m_NumFrames*m_NumDOFs, fp);

	fclose(fp);
#endif

	return 1;
}



// Save the motion data and user defined data in Mrdplot format,
// where "n" is the dimension of the user-defined data.
// You should also provide each variable's name and unit
int Motion::writeMRDfile(char *filename, float *data, char varNames[][256], char varUnits[][256], int n)
{

//DEBUG: Important, fix this function 
#if 0
	int i;
	FILE *fp;
   
	fp=fopen(filename,"wt");
	if(fp==NULL) return 0;

	fprintf(fp, "%d %d %d 1.00", m_NumFrames*(m_NumDOFs+n), m_NumDOFs+n, m_NumFrames);
	for(i=0;i<3;i++)
		fprintf(fp, "%s  m  ", StateIdx[i]);

	for(i=3;i<m_NumDOFs;i++)
		fprintf(fp, "%s  deg  ", StateIdx[i]);

	for(i=0;i<n;i++)
		fprintf(fp, "%s  %s  ", varNames[i], varUnits[i]);

	fprintf(fp, "\n");

	for(int f=0;f<m_NumFrames;f++)
	{
		fwrite(m_State[f], sizeof(float), m_NumDOFs, fp);
		fwrite(data+f*n, sizeof(float), n, fp);
	}

	fclose(fp);
#endif

	return 1;
}

