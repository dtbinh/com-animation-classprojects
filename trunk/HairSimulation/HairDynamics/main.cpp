/******************************************************************************
 Final Project

�۾�
�����Y:     ^
  �W   |    |
���U�k | <- | ->

����:
�e/�� 7/9
  �W   |   8
���U�k | 4 5 6

�ק� 2009-1-10
�N ray trace ���������}���� => ���۽Ь� �i��_log.txt
******************************************************************************/
#include	"main.h"

// �����ܼ� ********************************************************************/ 
GLuint texObject[TEX_NUM];	// texture object
mesh *object;
int winW, winH;		// �������j�p
//�ª�global�ܼ�:��W���@�~�ѤU��
// ����Mzoom in-out���ܼ�
Vector3 pos, oldPos;	// position, old position
// �վ���u�M����]�w���ܼ�
LightSource light;	//����
MaterialSetting material;
Vec4 *changeVecPtr = &light.pos;	// �O�ثe�|���ܪ��ѼƦV�q( Vec4 )
float *changeNumPtr = &light.pos.x;	// �O�ثe�|���ܪ��ѼƼƭ�
ChangeSettingType changeSetting = LIGHT;	//	�O�|���ܪ�����
Camera camera;		//��v��/�۾�
std::vector< mesh * > objectList; //������������
/******************************************************************************/


//~~~~~~~~~~ �����ܼ� global variable ~~~~~~~~~~~~
std::vector<Line> lineSet;
StrandSystem ps;
clock_t startTime = 0;		//���ɶ�
clock_t endTime = 0;
bool drawBVTree = false;	//�e bounding box �����P�Ҧ�
bool drawSBox = false;
bool drawPBall = false;		//�e ��collision �� ball
bool drawParticle = false;	//�e line segment �W�� particle
//~~~~~~~~~~ idle() ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void idle()
{
//#define see_time
#ifdef see_time
	clock_t start = clock();
#endif
	for( int i = 0; i < 10; i++ ){	// 0.001 => 10 step 1frame
		ps.step( 0.001 );
	}
#ifdef see_time
	clock_t end = clock();
	//static bool showT = true;
	//if( showT ){
		double interval = (double) ( end - start )/CLOCKS_PER_SEC;
		printf("10 step takes %g sec\n", interval);
		//showT = false;
	//}
#endif
	glutPostRedisplay();
	
}


void idle3()
{
	//static int j = 0;
	for( int i = 0; i < 1; i++ ){	// 0.001 => 10 step 1frame
		ps.step( 0.001 );
	}
	//j++;

	//if( j > 1000 ){
//		cout << "leaf box intersect"<<leafBox << endl;
		clock_t endTime = clock();
		double interval = (double) ( endTime - startTime )/CLOCKS_PER_SEC;
		printf("1000 frame w/ 10 step takes %g sec\n", interval);
		exit(0);
	//}
	

	glutPostRedisplay();
	
}
void idle2()
{
	static int j = 0;
	for( int i = 0; i < 10; i++ ){	// 0.001 => 10 step 1frame
		ps.step( 0.001 );
	}
	j++;

	if( j > 1000 ){
		clock_t endTime = clock();
		double interval = (double) ( endTime - startTime )/CLOCKS_PER_SEC;
		printf("1000 frame w/ 10 step takes %g sec\n", interval);
		exit(0);
	}
	glutPostRedisplay();
}

//~~~~~~~~~~ main() ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main( int argc, char **argv )
{
	
	//if( argc >= 2 ){
	//	object = new mesh( argv[1] );}
	//else{ cout<< "need mesh-obj arguement\n";		return 0; }

	mesh * mPtr;

	//mPtr = new mesh( "nice_bear.obj" );
	mPtr = new mesh("ball.obj");
	object = mPtr;
	mPtr->lightingProperty = GENERAL;
	objectList.push_back( mPtr );
	
	//��
	addLinesOnFace( mPtr->myFaceList[54], &lineSet );
	//addLinesOnFace( mPtr->myFaceList[55], &lineSet );
	//mPtr->myFaceList[38].printInfo();
	//for( vector<Face>::iterator it = mPtr->myFaceList.begin(); it != mPtr->myFaceList.end(); it++ )
	//{
	//	addLinesOnFace( *it, &lineSet ); 
	//}
	cout << lineSet.size() << endl;

	//~~~~ setup strands ~~~~
	ps.setup( lineSet, mPtr );
	// ����t�αq��l�]��í�w�����A ps.m_time => 8.4
    //cout << "����t�αq��l��m�]��í�w�����A" << endl;
	//for( int i = 0; i < 8400; i++ ){
	//	ps.step( 0.001f );
	//}
	//---------------------------------------------------
	//�]�wglut�e��
	glutInit( &argc, argv );
	winH = 300;
	winW = 300;
	glutInitWindowSize( winW, winH );
	glutInitWindowPosition( 200, 200 );
#define using_glut_double
#ifdef using_glut_double
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
#else
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB );
#endif
	glutCreateWindow( "final" );

	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyboard );
	glutSpecialFunc(processSpecialKeys );

	glutIdleFunc( idle );
	
	startTime = clock();
	//glutMouseFunc( mouse );
	//glutMotionFunc( mouseMotion );
	glutMainLoop();

	return 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Add Line segment on face ~~~~~~~~~~~~~~~~~~~~~~~
//�b�T���άY���I�W���@���u
//�I����m p = (1-u-v)p0 + u*p1 + v*p2	p0,p1,p2 �z�L Face ��
//�u�����׬O length, �`�@���X�q totalPieces
Line makeALine( const Face &face, float u, float v )
{
	//float length = 3.0f;
	int totalPieces = 30;
	float pieceLen = 3.0f / totalPieces;

	Vector3 p = (1-u-v)* (*face.vPtr[0]) + u* (*face.vPtr[1]) + v* (*face.vPtr[2]);
	Vector3 n = (1-u-v)* (*face.nPtr[0]) + u* (*face.nPtr[1]) + v* (*face.nPtr[2]);
	n.normalize();
	Vector3 piece = n *pieceLen;
	Line aLine;

	Vector3 pt = p;
	for( int i = 0; i < totalPieces; ++i, pt += piece ){
		aLine.pts.push_back( pt );
	}
	
	return aLine;
}

void addLinesOnFace( const Face &face, std::vector<Line> *lineArray )
{
	Line aLine;	
	//�]�w(u,v)����
	// u=0.0, v=0.0~0.9
	// u=0.1, v=0.0~0.8
	// ...
	// u=0.9, v=0.0
	// (u,v)��l�Ȳ���0.05
	int N = 9;
	float u,v;
	float d = 0.1f;
	float start = 0.05f;
	u = start;
	for( int i = 0; i < N; i++ ){
		v = start;
		for( int j = 0; j+i < N; j++ ){

			// add a line at p = (1-u-v)p0 + u*p1 + v*p2
			Line aLine = makeALine( face, u, v );
			lineArray->push_back( aLine );

			//printf("(%g,%g)\t", u, v );
			v += d;
		}
		u += d;
		//printf("\n");
	}
}

/*********************************************************************************/
/* opneGL */

void setLight()
{
	GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0 };
	GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0 };
	//GLfloat light_position[] = { 15.0, 15.0, 15.0, 0.0 };
	GLfloat light_position[] = {0.0, 30.0, -20.0 };
	GLfloat Ka[] = { 0.2, 0.2, 0.2 };
	GLfloat Kd[] = {0.9 , 0.9 , 0.9 };
	GLfloat Ks[] = {0.9 , 0.9 , 0.9 };
	GLfloat Ns = 160;
	glShadeModel(GL_SMOOTH);
	//glShadeModel(GL_FLAT);

	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light.pos.ptr );
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light.Ld.ptr );
	glLightfv(GL_LIGHT0, GL_SPECULAR, light.Ls.ptr );
	glLightfv(GL_LIGHT0, GL_AMBIENT, light.La.ptr );

	glMaterialfv(GL_FRONT, GL_AMBIENT , material.Ka.ptr );
	glMaterialfv(GL_FRONT, GL_DIFFUSE , material.Kd.ptr );
	glMaterialfv(GL_FRONT, GL_SPECULAR, material.Ks.ptr );
	glMaterialf(GL_FRONT, GL_SHININESS, material.Ns );
}

void reshape(GLsizei w, GLsizei h)
{
	winH = h;
	winW = w;
	// viewport
	glViewport(0, 0, winW, winH );
}
//~~~~~~~~~~ Display ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static bool drawLine = false;
int mark3;
void display()
{
	// projection
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 86.0, (GLdouble) winW/winH, 1.0, 1000.0 );	
	//�۾����Y�Mimage plane(����)�W�U�ݪ�������90

	//glFrustum( -38.0, 38.0, -30.0, 30.0, 1.0, 100.0 ); //���G���~
	//glOrtho( -38.0, 38.0, -30.0, 30.0, 1.0, 1000.0 );
	// viewing, modeling
	glMatrixMode(GL_MODELVIEW );
	glLoadIdentity();
	
	gluLookAt( camera.eyePos.x, camera.eyePos.y, camera.eyePos.z,		// eyePos
				camera.center.x, camera.center.y, camera.center.z,		// center, direction = center - eyePos
				camera.up.x, camera.up.y, camera.up.z );	// up
	//�����]�w
	setLight();
	// �]�M���e���� color
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClearDepth(1.0f);			//depth buffer setup
	glEnable( GL_DEPTH_TEST );	//enable depth test
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// �M�� buffer
	
	//�}�l�e�F��
	
	// mesh object ����
	// �i�H�Х� face �O�X��
	for( std::vector< mesh* >::const_iterator mPtr = objectList.begin(); 
		mPtr != objectList.end(); ++mPtr )
	{
		//~~~~~ �e bvTree ~~~~~
		if( drawBVTree ){
			assert( (*mPtr)->m_bvTree.root );
			(*mPtr)->m_bvTree.display();
		}

		//�e mesh
		int i = 0;
		for( std::vector< Face >::const_iterator fPtr = (*mPtr)->myFaceList.begin(); 
			fPtr != (*mPtr)->myFaceList.end(); ++fPtr )
		{
			//object->fPtr.materialPtr->printInfo(); //��my Face class
			glMaterialfv(GL_FRONT, GL_AMBIENT , fPtr-> materialPtr->Ka );
			glMaterialfv(GL_FRONT, GL_DIFFUSE ,	fPtr->materialPtr->Kd  );
			glMaterialfv(GL_FRONT, GL_SPECULAR,	fPtr->materialPtr->Ks );
			glMaterialf(GL_FRONT, GL_SHININESS,	fPtr->materialPtr->Ns );
			//glColor3i( 1,0,1);
			// �Хܭn�[ line segment ���T����

			//�e��u�ι���
			if(!drawLine){ 
				glBegin( GL_TRIANGLES );
			}else{
				if( ++i != mark3 ){
					glBegin( GL_LINE_LOOP );
				}else{
					glBegin( GL_TRIANGLES );
					float Kd[4] = {1, 1, 0, 1};
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE ,Kd  );
				}
			}

			for( int j = 0; j < 3; j++ ){
				//glNormal3fv( fPtr->nPtr[j]->ptr );
				glNormal3fv( fPtr->fnPtr->ptr );
				glVertex3fv( fPtr->vPtr[j]->ptr );
			}	
			glEnd();
		}
		
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// �e�u strand
#if 1
	//glEnable( GL_LINE_SMOOTH );
	if( drawSBox ){
		ps.drawStrandBoxes();	// �e bounding box
	}

	for( std::vector<Strand>::const_iterator sptr = ps.m_strandList.begin();
		sptr != ps.m_strandList.end(); ++sptr )
	{
		// �e strand-line
		glBegin( GL_LINE_STRIP );
		float rKd[4] = {1, 0, 0, 1};			// Red
		float cKd[4] = { 0.3, 1, 0, 1 };		// Green
		float yKd[4] = { 1, 1, 0.4, 1 };		// �L��
		float purple[4] ={ 1, 0.2, 0.8, 1 };	//purple
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, purple );
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT , purple );

		for( std::vector<Particle>::const_iterator pptr = sptr->pList.begin();
			pptr != sptr->pList.end(); ++pptr )
		{
			glVertex3fv(pptr->x.ptr);
		}
		glEnd();

		if( drawPBall ){
			// �e strand particle or Ball
			for( std::vector<Particle>::const_iterator pptr = sptr->pList.begin();
				pptr != sptr->pList.end(); ++pptr )
			{
				if( pptr->status == FREE ){
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cKd );
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT , cKd );
				}else{
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, rKd );
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT , rKd );
				}
				glPushMatrix();
				glTranslatef( pptr->x[0], pptr->x[1], pptr->x[2] );

				glutSolidSphere( P_BALL_R, 5, 5 );
				glPopMatrix();
			}
		}
		else if( drawParticle ){
			// �e strand particle or Ball
			for( std::vector<Particle>::const_iterator pptr = sptr->pList.begin();
				pptr != sptr->pList.end(); ++pptr )
			{
				if( pptr->status == FREE ){
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cKd );
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT , cKd );
				}else{
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, rKd );
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT , rKd );
				}
				glPushMatrix();
				glTranslatef( pptr->x[0], pptr->x[1], pptr->x[2] );

				glutSolidSphere( 0.02, 5, 5 );
				glPopMatrix();
			}
		}
	}
#endif

	// �e line-segment set �Ϊ�
#if 0
	for( std::vector<Line>::const_iterator linePtr = lineSet.begin();
		linePtr != lineSet.end(); linePtr++ )
	{
		glBegin( GL_LINE_STRIP );

		float lineKd[4] = {1, 0, 0, 1};
		float cKd[4] = { 0.3, 1, 0, 1 };// Green
		float yKd[4] = { 1, 1, 0.4, 1 };// �L��
		float zero[4] ={ 0, 0, 0, 1 };	//��
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, yKd );
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT , yKd );
		
		for( std::vector<Vector3>::const_iterator ptPtr = linePtr->pts.begin();
			ptPtr != linePtr->pts.end(); ptPtr++ )
		{
		//glColor3i( 1, 0, 0 );
			glVertex3fv(ptPtr->ptr);
		}
		glEnd();
	}
#endif
	glPushMatrix();
	glTranslatef( light.pos.x, light.pos.y, light.pos.z );
	glutSolidSphere( 1.0, 10, 10 );
	glPopMatrix();
	
	glFlush();
	glFinish();
#ifdef using_glut_double
	glutSwapBuffers();
#endif

}

//~~~~~~~~~~~ key & special key ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void keyboard( unsigned char key, int x, int y )
{
	static float m = 1;	// move camera how much
	bool redraw = true;
	static bool rayTraceMode = true;
	Vector3 zoom;
	//printf( "key %c #%d pressed\n", key, key );

	switch( key ){

		case 13:	//Enter
			for( int i = 0; i < 100; i++ ){
			ps.step( 0.001 );
			//glutPostRedisplay();
			}
			break;
		case 'g':
			for( int i = 0; i < 1000; i++ ){
				ps.step( 0.001 );
			}
			break;

		//~~~~�аOface number~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 27:	// Ecs
		case 'Q':
		case 'q':
			exit(1);
			break;
		//������ܼҦ�
		//case 'w':
		//	if( rayTraceMode ){
		//		glutDisplayFunc( display );
		//	}else{
		//		glutDisplayFunc( display1 );
		//	}
		//	rayTraceMode = !rayTraceMode;
		//	break;
		case 'W':
			drawLine = !drawLine;
			break;
		case 'T':
			cout << "ps m_time:" << ps.m_time << endl;
			break;
		//~~~~�аOface number~~~~~~~~~~~~~~~~~~~~~~~~~~
		case'f':
			printf("mark face no.%d\n", mark3 );
			mark3++;
			assert(object);
			if( mark3 > object->fTotal ){
				mark3 = 0;
			}
			break;
		case 'F':
			printf("mark face no.%d\n", mark3 );
			mark3--;
			if( mark3 < 0 ){
				assert(object);
				mark3 = object->fTotal - 1;
			}
			break;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		case 'k':
			takePicture();
			break;
		// ���Y�Y��
		case 'i':
			VecScale( camera.eyeDir, 0.5, zoom );
			VecAdd( camera.eyePos, zoom, camera.eyePos );
			break;
		case 'I':	
			VecScale( camera.eyeDir, 0.5, zoom );
			VecSub( camera.eyePos, zoom, camera.eyePos );			
			break;
		// ����
		case 'r':
			//spinEyeDir += .3f;
			break;
		//����camera
		case '6':
			camera.turnRightLeft(m);
			break;
		case '4':
			camera.turnRightLeft(-m);
			break;
		case '8':
			camera.turnUpDown(m);
			break;
		case '5':
			camera.turnUpDown(-m);
			break;
		case'7':
			camera.moveForthBack( m );
			break;
		case '9':
			camera.moveForthBack( -m );
			break;
		//������,�M prossSpecialKey() �@�_��
		case 127:	// Delete
			light.pos[0] -= m;
			break;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//case '1':
		//	changeNumPtr = &changeVecPtr->r;
		//	break;
		//case '2':
		//	changeNumPtr = &changeVecPtr->g;
		//	break;
		//case '3':
		//	changeNumPtr = &changeVecPtr->b;
		//	break;
		//case '0':
		//	changeNumPtr = &changeVecPtr->a;
		//	break;

		// �L�X�ثe���u����]�w
		case 'P':
		case ' ':
			printf("Ray Trace %s\n", rayTraceMode ? "on":"off");
			printf("-----------camera.eyePos info-----------\n");
			printf("pos:");
			camera.eyePos.printInfo();
			printf("camera.center:");
			camera.center.printInfo();
			printf("dir:");
			camera.eyeDir.printInfo();
			printf("camera.up:");
			camera.up.printInfo();
			printf("\n--------------------------------------\n");
			printf("curret light:\n");
			printf( "specular (r,g,b,a) = (%g, %g, %g, %g)\n", light.Ls.r, light.Ls.g, light.Ls.b, light.Ls.a );
			printf( "diffuse (r,g,b,a) = (%g, %g, %g, %g)\n", light.Ld.r, light.Ld.g, light.Ld.b, light.Ld.a );
			printf( "ambient (r,g,b,a) = (%g, %g, %g, %g)\n", light.La.r, light.La.g, light.La.b, light.La.a );
			printf( "position (r,g,b,a) = (%g, %g, %g, %g)\n", light.pos.r, light.pos.g, light.pos.b, light.pos.a );
			printf("---------------------------------------------------------------\n");
			redraw = false;
			break;
		default:
			redraw = false;
			break;
	}
	if( redraw )
		glutPostRedisplay();
}

void processSpecialKeys(int key, int x, int y) 
{
	static float m = 1.0f;	// move camera how much
	
	bool redraw = true;
	
	switch(key) {
		//����camera ��
		case GLUT_KEY_RIGHT:
			camera.moveRightLeft(m);
			break;
		case GLUT_KEY_LEFT:
			camera.moveRightLeft(-m);
			break;
		case GLUT_KEY_UP:
			camera.moveUpDown( m );
			break;
		case GLUT_KEY_DOWN:
			camera.moveUpDown( -m );
			break;

		case GLUT_KEY_PAGE_UP:
			light.pos[2] += m;
			break;
		case GLUT_KEY_PAGE_DOWN:
			light.pos[0] += m;
			break;
		case GLUT_KEY_HOME:
			light.pos[1] += m;
			break;
		case GLUT_KEY_END:
			light.pos[1] -= m;
			break;
		case GLUT_KEY_INSERT:
			light.pos[2] -= m;
			break;

		//~~~~ demo �}�� drawPBall, drawBVTree, drawSBox
		case GLUT_KEY_F1:
			drawPBall = !drawPBall;
			break;
		case GLUT_KEY_F2:
			drawBVTree = !drawBVTree;
			break;
		case GLUT_KEY_F3:
			drawSBox = !drawSBox;
			break;
		case GLUT_KEY_F4:
			drawParticle = !drawParticle;
			break;
		//~~~~~~~~
		default:
			redraw = false;
			break;
	}
	if( redraw ){
		glutPostRedisplay();
	}
}

void takePicture()
{
	static int i = 0;

	GLubyte *bitPtr = 0;
	//BITMAPFILEHEADER bh;
	BITMAPINFO info;

	bitPtr = ReadDIBitmap( info );
	if( bitPtr == 0 ){
		cerr<<"read pixel error\n";
	}
	else{
		//cout << "read pixel succeeded\n";
	}
	char picName[ 40 ];
	++i;
	sprintf( picName, "pic/take_picture_%04d.bmp", i );
	//cout << "call takePic() " << i << endl;
	SaveDIBitmap( picName, &info, bitPtr );
}

//******************************************************************************************


//******************************************************************************************






