//////////////////////////////////////////////////////////////////////
//
// 修改了 mesh.cpp 11/01
//
// mesh.cpp: implementation of the mesh class.
//
//////////////////////////////////////////////////////////////////////

#include "mesh.h"
#include <iostream>
#include <cassert>
#include <cmath>
const char* obj_database = "";	// 定義 mesh 的預設目錄

#pragma warning(disable:4786)	// microsoft stupid bug!!

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

mesh::mesh(const char* obj_file)
{
	matTotal = 0;		// mat[0] reserved for default meterial
	vTotal = tTotal = nTotal = fTotal = 0;
	fnList = 0;
	vAdjFList = 0;
	
	Init(obj_file);
}

mesh::mesh()
{
	matTotal = 0;			
	vTotal = tTotal = nTotal = fTotal = 0;
	fnList = 0;
	vAdjFList = 0;
}

mesh::~mesh()
{
	if( fnList != 0 )
		delete[] fnList;
	if( vAdjFList != 0 )
		delete[] vAdjFList;
}

void mesh::LoadMesh(string obj_file)
{
	char	token[100], buf[100], v[5][100];	// v[5] 表示一個 polygon 可以有 5個 vertex
	float	vec[3];

	int	n_vertex, n_texture, n_normal;
	int	cur_tex = 0;				// state variable: 目前所使用的 material

	scene = fopen(obj_file.c_str(),"r");
	s_file = obj_file;

	if (!scene) 
	{
		cout<< string("Can not open object File \"") << obj_file << "\" !" << endl;
		return;
	}

	cout<<endl<<obj_file<<endl;
		
	while(!feof(scene))
	{
		token[0] = NULL;
		fscanf(scene,"%s", token);		// 讀 token

		if (!strcmp(token,"g"))
		{
			fscanf(scene,"%s",buf);
		}

		else if (!strcmp(token,"mtllib"))
		{
  			fscanf(scene,"%s", mat_file);
			LoadTex(string(obj_database) + string(mat_file));
		}

		else if (!strcmp(token,"usemtl"))
		{
			fscanf(scene,"%s",buf);
			cur_tex = matMap[s_file+string("_")+string(buf)];
		}

		else if (!strcmp(token,"v"))
		{
			fscanf(scene,"%f %f %f",&vec[0],&vec[1],&vec[2]);
			vList.push_back(Vector3(vec));
		}

		else if (!strcmp(token,"vn"))
		{
			fscanf(scene,"%f %f %f",&vec[0],&vec[1],&vec[2]);
			nList.push_back(Vector3(vec));
		}
		else if (!strcmp(token,"vt"))
		{
			fscanf(scene,"%f %f",&vec[0],&vec[1]);
			tList.push_back(Vector3(vec));
		}

		else if (!strcmp(token,"f"))
		{
			int i;
			for (i=0;i<3;i++)		// face 預設為 3，假設一個 polygon 都只有 3 個 vertex
			{
				fscanf(scene,"%s",v[i]);
				//printf("[%s]",v[i]);
			}
			//printf("\n");
		  
			Vertex	tmp_vertex[3];		// for faceList structure

			for (i=0;i<3;i++)		// for each vertex of this face
			{
				char str[20], ch;
				int base,offset;
				base = offset = 0;

				// calculate vertex-list index
				while( (ch=v[i][base+offset]) != '/' && (ch=v[i][base+offset]) != '\0')
				{
					str[offset] = ch;
					offset++;
				}
				str[offset] = '\0';
				n_vertex = atoi(str);
				base += (ch == '\0')? offset : offset+1;
				offset = 0;

				// calculate texture-list index
				while( (ch=v[i][base+offset]) != '/' && (ch=v[i][base+offset]) != '\0')
				{
					str[offset] = ch;
					offset++;
				}
				str[offset] = '\0';
				n_texture = atoi(str);	// case: xxx//zzz，texture 設為 0 (tList 從 1 開始)
				base += (ch == '\0')? offset : offset+1;
				offset = 0;

				// calculate normal-list index
				while( (ch=v[i][base+offset]) != '\0')
				{
					str[offset] = ch;
					offset++;
				}
				str[offset] = '\0';
				n_normal = atoi(str);	// case: xxx/yyy，normal 設為 0 (nList 從 1 開始)

				tmp_vertex[i].v = n_vertex;
				tmp_vertex[i].t = n_texture;
				tmp_vertex[i].n = n_normal;
				tmp_vertex[i].m = cur_tex;
			}

			faceList.push_back(FACE(tmp_vertex[0],tmp_vertex[1],tmp_vertex[2]));
		}

		else if (!strcmp(token,"#"))	  // 註解
			fgets(buf,100,scene);

//		printf("[%s]\n",token);
	}

	if (scene) fclose(scene);

	vTotal = int( vList.size());
	nTotal = int( nList.size());
	tTotal = int(tList.size());
	fTotal = int(faceList.size());
	printf("vetex: %d, normal: %d, texture: %d, triangles: %d\n",vTotal, nTotal, tTotal, fTotal);
}

void mesh::LoadTex(string tex_file)
{
	//char	token[100], buf[100], v1[100], v2[100], v3[100];
	char	token[100], buf[100];
	//float	x,y,z,r,g,b;
	float	r,g,b;


	//int	n_vertex, n_texture, n_normal;		// temp 的 current data
	//int	cur_tex;				// 儲存接著下去的 polygon 是用哪個 texture

	texture = fopen(tex_file.c_str(),"r");
	t_file = tex_file;

	if (!texture) 
	{
		cout << "Can't open material file \"" << tex_file << "\"!" << endl;
		return;
	}

	cout<<tex_file<<endl;

	int cur_mat;

	while(!feof(texture))
	{
		token[0] = NULL;
		fscanf(texture,"%s", token);		// 讀 token

		if (!strcmp(token,"newmtl"))
		{
			fscanf(texture,"%s",buf);
			cur_mat = matTotal++;					// 從 mat[1] 開始，mat[0] 空下來給 default material 用
			matMap[s_file+string("_")+string(buf)] = cur_mat; 	// matMap["material_name"] = material_id;
		}

		else if (!strcmp(token,"Ka"))
		{
			fscanf(texture,"%f %f %f",&r,&g,&b);
			mat[cur_mat].Ka[0] = r;
			mat[cur_mat].Ka[1] = g;
			mat[cur_mat].Ka[2] = b;
			mat[cur_mat].Ka[3] = 1;
		}

		else if (!strcmp(token,"Kd"))
		{
			fscanf(texture,"%f %f %f",&r,&g,&b);
			mat[cur_mat].Kd[0] = r;
			mat[cur_mat].Kd[1] = g;
			mat[cur_mat].Kd[2] = b;
			mat[cur_mat].Kd[3] = 1;
		}

		else if (!strcmp(token,"Ks"))
		{
			fscanf(texture,"%f %f %f",&r,&g,&b);
			mat[cur_mat].Ks[0] = r;
			mat[cur_mat].Ks[1] = g;
			mat[cur_mat].Ks[2] = b;
			mat[cur_mat].Ks[3] = 1;
		}

		else if (!strcmp(token,"Ns"))
		{
			fscanf(texture,"%f",&r);
			mat[cur_mat].Ns = r;
		}

		else if (!strcmp(token,"#"))	  // 註解
			fgets(buf,100,texture);

//		printf("[%s]\n",token);
	}

	printf("total material:%d\n",matMap.size());

	if (texture) fclose(texture);
}

void mesh::Init(const char* obj_file)
{
	float default_value[3] = {1,1,1};

	vList.push_back(Vector3(default_value));	// 因為 *.obj 的 index 是從 1 開始
	nList.push_back(Vector3(default_value));	// 所以要先 push 一個 default value 到 vList[0],nList[0],tList[0]
	tList.push_back(Vector3(default_value));

	// 定義 default meterial: mat[0]
	mat[0].Ka[0] = 0.0f; mat[0].Ka[1] = 0.0f; mat[0].Ka[2] = 0.0f; mat[0].Ka[3] = 1.0f; 
	mat[0].Kd[0] = 1.0f; mat[0].Kd[1] = 1.0f; mat[0].Kd[2] = 1.0f; mat[0].Kd[3] = 1.0f; 
	mat[0].Ks[0] = 0.8f; mat[0].Ks[1] = 0.8f; mat[0].Ks[2] = 0.8f; mat[0].Ks[3] = 1.0f;
	mat[0].Ns = 32;
	matTotal++;

	LoadMesh(string(obj_file));		// 讀入 .obj 檔 (可處理 Material)
	
	//算vertex的平均normal	(facet normal)
	CalculateFn();
	//create myFaceList
	buildMyFaceList();
}

//建myFaceList
void mesh::buildMyFaceList()
{
	assert( vTotal > 0 );
	assert( fTotal > 0 );
	assert( fnList != 0 );

	for( int i = 0; i < fTotal; i++ ){
		Face f;
		f.materialPtr = &mat[ faceList[i][0].m ];
		for( int w = 0; w < 3; w++ ){
			f.vPtr[w] = &vList[ faceList[i][w].v ];
			f.nPtr[w] = &nList[ faceList[i][w].n ];
			f.tPtr[w] = &tList[ faceList[i][w].t ];

		}
		f.fnPtr = &fnList[i];
		f.cPtr = &triCList[i];
		myFaceList.push_back( f );
	}
	//build bv-tree
	
	m_bvTree.buildTree( myFaceList, fTotal );
}



/** 2009-01-12 另有用途 */
void mesh::CalculateFn()
{
	assert( vTotal > 0 );
	assert( fTotal > 0 );

	if( fnList != 0 ){
		delete[] fnList;
	}
	if( vAdjFList != 0 ){
		delete[] vAdjFList;
	}
	//if( triCList != 0 ){
	//	delete[] triCList;
	//}
	
	//triCList = new Vector3[ fTotal ];
	fnList = new Vector3[ fTotal ];
	assert( fnList );
	//assert( triCList );
	for( int f = 0; f < fTotal; f++ ){
		
		Vector3 a = vList[ faceList[f][1].v ] - vList[ faceList[f][0].v ];
		Vector3 b = vList[ faceList[f][2].v ] - vList[ faceList[f][0].v ];
		// 注意外積的方向要和平面相同
		Vector3 n = crossProduct( a, b);
		n.normalize();
		fnList[f] = n;

		//triCList[f] = (vList[ faceList[f][0].v ] + vList[ faceList[f][0].v ] + vList[ faceList[f][0].v ] )/3;
	}
}//close function: calculateFn

/***********************************************
函式
算出每個頂點的 facet noraml (多面體法向量)
也就是一個頂點所有相鄰的三角形平面法向量的平均
放置在
Vec3 fnList[ vTotal ] 中
fnList[v] 就是第 v 的頂點的法向量
例如
v: vertex index
glNormal3fv( fnList[v] );
glVertex3fv( vList[v] );


call 之前 mesh 的 vList, faceList 已經建好了
*************************************************/
#if 0
void mesh::CalculateFn()
{
	assert( vTotal > 0 );
	assert( fTotal > 0 );

	if( fnList != 0 ){
		delete[] fnList;
	}
	if( vAdjFList != 0 ){
		delete[] vAdjFList;
	}

	
	fnList = new Vector3[ vTotal ];
	vAdjFList = new vector<int> [ vTotal ];
	// vList 編號從1開始
	for( int i = 0; i < fTotal; i++ ){
		for( int j = 0; j < 3; j++ ){
			vAdjFList[ faceList[i][j].v ].push_back( i );	
		}
	}

#ifdef DEBUG_1
	for( int i = 1; i < vTotal; i++ ){
		printf( "%d adj f:", i );
		for( int j = 0; j < vAdjFList[i].size(); j++ ){
			printf( " %d", vAdjFList[i][j] );
		}
		printf("\n");
	}
#endif

	Vector3 *faceNormal = new Vector3[ fTotal ];
	for( int f = 0; f < fTotal; f++ ){
		Vector3 a, b, n;
		VecSub( vList[ faceList[f][1].v ], vList[ faceList[f][0].v ], a );
		VecSub( vList[ faceList[f][2].v ], vList[ faceList[f][0].v ], b );
		// 注意外積的方向要和平面相同
		VecCross( a, b, n );
		NormalizeVec( n );
		faceNormal[f] = n;
	}

#ifdef DEBUG_1
	for( int  f = 0; f < fTotal; f++ ){
		printf( "%g,%g,%g \n", faceNormal[f][0], faceNormal[f][1], faceNormal[f][2] );
	}


	for( int v = 1; v < vTotal; v++ ){
		printf( "v: " );
		for( int i = 0; i < vAdjFList[v].size(); i++ ){

		printf( "%g,%g,%g \n", faceNormal[ vAdjFList[v][i] ][0], faceNormal[ vAdjFList[v][i] ][1], faceNormal[ vAdjFList[v][i] ][2] );

		}
		printf( "\n" );
	}
#endif

	// calculate facet normal: average of all adjacent face's normal
	for( int v = 1; v < vTotal; v++ ){

		Vector3 n( 0.0, 0.0, 0.0 );
		for( unsigned int i = 0; i < vAdjFList[v].size(); i++ ){

			VecAdd( n, faceNormal[ vAdjFList[v][i] ], n );

		}
		NormalizeVec( n );
		fnList[v] = n;

#ifdef DEBUG_1
		printf( "%g,%g,%g \n", fnList[v][0], fnList[v][1], fnList[v][2] );
#endif
	}

	if( faceNormal != 0 )
		delete[] faceNormal;
}//close function: calculateFn
#endif

Face::Face()
:materialPtr(0), fnPtr(0)
{
	for( int i = 0; i < 3; i++ ){
		vPtr[i] = nPtr[i] = tPtr[i] = NULL;
	}
}

Face::Face( const Face & face )
{
	for( int i = 0; i < 3; i++ ){
		vPtr[i] = face.vPtr[i];
		nPtr[i] = face.nPtr[i];
		tPtr[i] = face.tPtr[i];
	}
	fnPtr = face.fnPtr;
	cPtr = face.cPtr;
	materialPtr = face.materialPtr;
}