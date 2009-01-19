///////////////////////////////////////////////////////////////////////////////
// �ק� mesh.h 
//	11/01	��vertex����normal:�۾F��������normal
//			�W�[ fnList, vAdjFList, calculateFn()	
//			���� Vec3 class 
//	12/9	���� Face class:face����data���pointer�s. �W�[ myFaceList
//			�b�� Ray-Triangle(Face) �O�_�����,���ζ�mesh object��pointer��reference
//
// mesh.h: interface for the mesh class.
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESH_H__07F55FBE_D4DD_451E_B587_680CFA0DE9C4__INCLUDED_)
#define AFX_MESH_H__07F55FBE_D4DD_451E_B587_680CFA0DE9C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
using namespace std;
#include	"Vector3.h"		//Vector3
#include	"mat_face.h"	// material face
#include	"aabb.h"		// aabb bv-tree
#include "AppMesh.h"
// ���u�]�w�Ѽ�
class LightSource{
public:
	Vec4 Ls;
	Vec4 Ld;
	Vec4 La;
	Vec4 pos;

	LightSource(){
		Ls = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		Ld = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		La = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
		pos = Vec4(-9, 25, 20, 0.0f);	//animation final demo v2
	}
};
// ����]�w�Ѽ�
class MaterialSetting{
public:
	Vec4 Ka;
	Vec4 Kd;
	Vec4 Ks;
	float Ns;

	MaterialSetting(){
		Ka = Vec4( 0.2f, 0.2f, 0.2f, 1.0f );
		Kd = Vec4(0.6f , 0.6f , 0.6f, 1.0f );
		Ks = Vec4(0.6f , 0.6f , 0.6f, 1.0f );
		Ns = 30.0;	// 0-128
	}
};



enum LightingProperty{			//���饴��������S��
	GENERAL = 0, MIRROR = 1
};
//~~~~~~~~~~~ mesh with bv-tree ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class mesh  
{
//public:

	class Vertex		// �x�s�� vertex �� property
	{
	public:
		int v;		// vertex (index of vList)
		int n;		// normal (index of nList)
		int t;		// texture (index of tList)
		int m;		// material (index of material)
		Vertex() {};
		Vertex(int v_index, int n_index, int t_index=0, int m_index=0)
		{
			v = v_index;
			n = n_index;
			t = t_index;
			m = m_index;
		}
	};



	class FACE		// faceList �� structure
	{
	public:
		Vertex v[3];		// 3 vertex for each face
		FACE (Vertex &v1, Vertex &v2, Vertex &v3) 
		{
			v[0] = v1; 
			v[1] = v2;
			v[2] = v3;
		}
		Vertex& operator[](int index)
		{
			return v[index];
		}
	};

public:
	/////////////////////////////////////////////////////////////////////////////
	// Loading Object
	/////////////////////////////////////////////////////////////////////////////

	FILE	*scene, *texture;
	string  s_file, t_file;
	char	mat_file[50];		// matetial file name

	int		matTotal;	// total material 
	map<string,int> matMap;		// matMap[material_name] = material_ID
	material	mat[100];	// material ID (�C�� mesh �̦h�� 100 �� material)	

	vector<AppVector3>	vList;		// Vertex List (Position) - world cord.
	vector<AppVector3>	nList;		// Normal List
	vector<AppVector3>	tList;		// Texture List
	vector<FACE>	faceList;	// Face List
	// vList, nList, tList �s���q1�}�l, 0���O���|�Ψ쪺
	// faceList �s���q 0 �}�l
	AppVector3 * fnList;			// facet normal list
	AppVector3 * triCList;			// triangle's cetroid list
	vector<int> *vAdjFList;		// vertex's adjacent faces list
	vector<Face> myFaceList;	// my face data-structure
	LightingProperty lightingProperty;
	AabbTree m_bvTree;				// BV-tree

	int	vTotal, tTotal, nTotal, fTotal;

	void	LoadMesh(string scene_file);
	void	LoadMesh(CMesh* sourceMesh);
	void	LoadTex(string tex_file);

	mesh(const CMesh* sourceMesh);
	mesh(const char* obj_file);
	virtual ~mesh();

	void Init(const char* obj_file);
	void Init(const CMesh* sourceMesh);
	//�ۤv�ק諸
	void CalculateFn();			//��vertex������normal (�C�Ӭ۾F��face��normal����)
	void buildMyFaceList();		//��myFaceList

};

#endif // !defined(AFX_MESH_H__07F55FBE_D4DD_451E_B587_680CFA0DE9C4__INCLUDED_)
