#if 0
void addLineOnFace()// const Face &face )
{
	//設定(u,v)的值
	int N = 10;
	float u,v,d;
	d = 0.1f;	// 1.0 / (N + 1)
	u = 0.05;
	for( int i = 0; i < N; i++ ){
		v = 0.05f;
		for( int j = 0; j+i < N; j++ ){
			//printf("(%g,%g)\t", u, v );
			v += 0.1f;
		}
		u += 0.1f;
		//printf("\n");
	}
}
#endif


#if 0
class Scenery{
public:
	std::vector <mesh *> objectList; //場景中的東西
	int totalObjects;
	LightSource light;
};
#endif

#if 0
class PixelColorPainter{
public:
	mesh *	meshPtr;
	LightSource *lightSourcePtr;
	Ray inRay;

	Vec3 calcColor( const Vec3 &ray )
	{
		Vec3 point;
		float uv[2];
		Face *iFacePtr = 
			getNearestIntersection_v2(ray, meshPtr->myFaceList, &point, uv );

		Vec3 pixelColor = computeColor( point, iFacePtr );
	}

	Vec3 computeColor( const Vec3 &point, Face *iFacePtr, float uv[2] )
{	
	Vec3 color( 0.0, 0.0, 0.0 );	//預設黑色
	if( iFacePtr != NULL ){	//有intersect
		Ray lightRay;	//out ray
		lightRay.start = point;
		Vec3 hitPoint;
		Vec3 d_L = Vec3( lightSourcePtr->pos4.ptr ) - point;
		lightRay.dir = normalize( d_L );
		float uv[2];
		// 再次找到交錯的face不要是自己
		Face *iFace = 
			getNearestIntersection( lightRay, object->myFaceList, &hitPoint );
		if( iFace != 0 ){
			Vec3 d_hit = hitPoint - point;
			if( d_L.length() >= d_hit.length() ){
				//under shadow, only ambient term
				for( int i = 0; i < 3; i++ ){
					color[i] += iFacePtr->materialPtr->Ka[i] * light.La[i];
				}
				return color;
			}
		}
		//add direct lighting
		Vec3 &n = *(iFacePtr->nPtr[0]);
		float cos1 = n * lightRay.dir;
		// diffuse
		if( cos1 > FLT_EPSILON ){
			for( int i = 0; i < 3; i++ ){
				color[i] += iFacePtr->materialPtr->Kd[i] * light.Ld[i] * cos1;
			}
		}
		for( int i = 0; i < 3; i++ ){
			color[i] += iFacePtr->materialPtr->Ka[i] * light.La[i];
		}
	}
	return color;
}
};
#endif

#if 0
//2008-12-10 compute color: normal 沒做內插
Vec3 computeColor( const Vec3 &point, const Vec3 &inRayDir, Face *iFacePtr )
{	
	Vec3 color( 0.0, 0.0, 0.0 );	//預設黑色
	if( iFacePtr != NULL ){	//有intersect
		Ray ray;
		ray.start = point;
		Vec3 hitPoint;
		Vec3 d_L = Vec3( light.pos4.ptr ) - point;
		ray.dir = normalize( d_L );

		// 再次找到交錯的face不要是自己
		Face *iFace = 
			getNearestIntersection( ray, object->myFaceList, &hitPoint );
		if( iFace != 0 ){
			Vec3 d_hit = hitPoint - point;
			if( d_L.length() >= d_hit.length() ){
				//under shadow, only ambient term
				for( int i = 0; i < 3; i++ ){
					color[i] += iFacePtr->materialPtr->Ka[i] * light.La[i];
				}
				return color;
			}
		}

		//add direct lighting
		for( int i = 0; i < 3; i++ ){
			color[i] += iFacePtr->materialPtr->Ka[i] * light.La[i];
		}
		// diffuse
		Vec3 &n = *(iFacePtr->nPtr[0]); //normal做內插
		float cos1 = n * ray.dir;
		if( cos1 > EPSILON ){
			for( int i = 0; i < 3; i++ ){
				color[i] += iFacePtr->materialPtr->Kd[i] * light.Ld[i] * cos1;
			}
			// specular
			Vec3 L_n = (d_L * n) * n;//L垂直分量
			Vec3 R = L_n - ( d_L - L_n ); //反射方向R = L_n - L_t.	L_t = d_L - L_n
			R.normalize();
			float cos2 = -R * inRayDir;
			if( cos2 > EPSILON ){
				for( int i = 0; i < 3; i++ ){
					color[i] += iFacePtr->materialPtr->Ks[i] * light.Ls[i] * pow( cos2, iFacePtr->materialPtr->Ns );
				}
			}
		}
		
	}
	return color;
}
#endif

#if 0
void display1_12_11()
{
	glDisable(GL_DEPTH_TEST);	//關掉深度測試
	glDisable( GL_LIGHTING );	//不打光

	// projection
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	//glOrtho( 0.0, 1, 0.0, 1, -1.0, 1.0 );
	glOrtho( 0.0, winW, 0.0, winH, -1.0, 1.0 );

	//gluOrtho2D( 0.0, (GLdouble)winW, 0.0, (GLdouble)winH );
	//gluPerspective( 45.0, (GLdouble) winW/winH, 10, 1000.0 );
	// viewing, modeling
	glMatrixMode(GL_MODELVIEW );
	glLoadIdentity();
	//double W_2 = (double)winW/2;
	//double H_2 = (double)winH/2;

	//gluLookAt( W_2, H_2, 1.0, W_2, H_2, 0.0, 0, 1, 0 );
	gluLookAt( 0, 0, 1.0, 0, 0, 0.0, 0, 1, 0 );

	// 設清除畫布的 color
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClear( GL_COLOR_BUFFER_BIT );
	
	//camera.eyePos = Vec3( 0, 0, 2.1 );
	//camera.center = Vec3( 0, 0, 0 );
	
	float near_scalar = 1.0, right = 1.0, top = 1.0;
	//camera.eyeDir = camera.center - camera.eyePos; //camera 自己知道 eyeDir
	Vec3 eyeDir = camera.center - camera.eyePos;
	RayGenerator rayGen( winW, winH, right, top, near_scalar );
	rayGen.setEyeInfo( &(camera.eyeDir), &camera.eyePos, &camera.up );

//#define DEBUG
#ifdef DEBUG
	FILE *fp;
	fp = fopen( "debug.txt", "w+" );
#endif
	
	glBegin( GL_POINTS );
	for( int i = 0; i < winW; i++ ){
		for( int j = 0; j < winH; j++ ){
			Ray ray = rayGen.generate( i, j );
			Vec3 point;
			float uv[2];
			Face *iFacePtr = 
				getNearestIntersection_v2(ray, object->myFaceList, &point, uv );

			Vec3 pixelColor = computeColor( point, ray.dir, iFacePtr);
#ifdef DEBUG
			pixelColor.printInfo( fp );
#endif
			glColor3fv( pixelColor.ptr );
			//glVertex2f( (float)i/winW, (float)j/winH );
			glVertex3i( i, j, 0 );

		}
#ifdef DEBUG
		fprintf(fp, "\n");
#endif
	}
	glEnd();
	
	printf("finish\n");
	glFlush();
}
#endif
#if 0
void display_12_10_DEBUG()
{
	glDisable( GL_LIGHTING );

	// projection
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	//glOrtho( 0.0, 1, 0.0, 1, -1.0, 1.0 );
	glOrtho( 0.0, winW, 0.0, winH, -1.0, 1.0 );

	//gluOrtho2D( 0.0, (GLdouble)winW, 0.0, (GLdouble)winH );
	//gluPerspective( 45.0, (GLdouble) winW/winH, 10, 1000.0 );
	// viewing, modeling
	glMatrixMode(GL_MODELVIEW );
	glLoadIdentity();
	double W_2 = (double)winW/2;
	double H_2 = (double)winH/2;

	//gluLookAt( W_2, H_2, 1.0, W_2, H_2, 0.0, 0, 1, 0 );
	gluLookAt( 0, 0, 1.0, 0, 0, 0.0, 0, 1, 0 );

	// 設清除畫布的 color
	glShadeModel( GL_FLAT );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClear( GL_COLOR_BUFFER_BIT );
	

	//eyePos = Vec3( 0, 0, 2.1 );
	//center = Vec3( 0, 0, 0 );
	
	float near_scalar = 1.0, right = 1.0, top = 1.0;
	eyeDir = center - eyePos;
	RayGenerator rayGen( winW, winH, right, top, near_scalar );
	rayGen.setEyeInfo( &eyeDir, &eyePos, &up );

#define DEBUG
#ifdef DEBUG
	FILE *fp;
	fp = fopen( "debug.txt", "w+" );
#endif
	
	glBegin( GL_POINTS );
	for( int i = 0; i < winW; i++ ){
		for( int j = 0; j < winH; j++ ){
			Ray ray = rayGen.generate( i, j );
			Vec3 point;
			Face *iFacePtr = 
				getNearestIntersection(ray, object->myFaceList, &point );

			Vec3 pixelColor = computeColor( point, ray.dir, iFacePtr);
#ifdef DEBUG
			pixelColor.printInfo( fp );
#endif
			glColor3fv( pixelColor.ptr );
			//glVertex2f( (float)i/winW, (float)j/winH );
			glVertex2f( i, j );

		}
#ifdef DEBUG
		fprintf(fp, "\n");
#endif
	}
	glEnd();
	
	printf("finish\n");
	glFlush();
}
#endif

#if 0
void display_12_9_test_pixel()
{
	// projection
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0.0, 1, 0.0, 1, -1.0, 1.0 );
	
	// viewing, modeling
	glMatrixMode(GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0, 1, 0 );

	// 設清除畫布的 color
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClear( GL_COLOR_BUFFER_BIT );
	
	glBegin( GL_POINTS );
	for( int i = 0; i < winW; i++ ){
		for( int j = 0; j < winH; j++ ){
			glColor3f( 1, 0, 1);
			glVertex2f( (float)i/winW, (float)j/winH );
		}
	}
	glEnd();
	printf("finish\n");
	glFlush();
}
#endif

void test_12_8()
{
//#define DEBUG_12_8
#ifdef DEBUG_12_8
	// 測rayTriangleIntersect
	printf("-------------------test ray_triangle_intersect---------------------------------\n");
	Vec3 v0(-1, 1, 0);
	Vec3 v1(1, -1, 0);
	Vec3 v2( 1, 1, 0);
	Vec3 e1 = v1 - v0;
	Vec3 e2 = v2 - v0;
	Vec3 tempPoint;
	Vec3 tuv;
	float distance;
	Ray ray;
	ray.start = Vec3( 0.5, 0.5, 1 );
	ray.dir = Vec3( 0, 0, -1 );
	ray.printInfo();
	bool inter = rayTriangleIntersect( ray, v0, e1, e2, &tempPoint, &distance );
	if( inter ){
		tempPoint.printInfo();
		printf("\tdist:%g", distance);
	}
	printf("\n---------------------------------------------------------------------------------\n");
	//測RayGenerator
	eyeDir = Vec3( 0, 0, -1 );
	eye = Vec3( 0, 0, 40 );

	//eyeDir = c - eye;
	RayGenerator rayGenerator( 2, 2, 1, 1, 39 );
	rayGenerator.setEyeInfo( &eyeDir, &eye, &up );
	
	Vec3 Right(1, 0, 0);
	Vec3 Top( 0, 1, 0 );
	Vec3 Near( 0, 0, -2 );
	Vec3 eyePos( 0, 0, 2 );
	int W = 2, H = 2;
	for( int i = 0; i < 2; i++ ){
		for( int j = 0; j < 2; j++ ){
			//Ray ray = generateRay( i, j, W, H, 10, 10, eyePos, Right, Top, Near );
			//ray.printInfo();
			Ray ray2  = rayGenerator.generate( i, j );
			ray2.printInfo();
		}
	}
	//return 0;

#endif
}
void test_12_9()
{
	//#define DEBUG_12_9
#ifdef DEBUG_12_9
	//測 getNearestIntersection()
	eyeDir = Vec3( 0, 0, -1 );
	eye = Vec3( 0, 0, 2 );

	//eyeDir = c - eye;
	RayGenerator rayGenerator( 2, 2, 0.2, 0.2, 1 );
	rayGenerator.setEyeInfo( &eyeDir, &eye, &up );
	
	Face *iFacePtr = NULL;
	Vec3 point;
	float dist;
	for( int i = 0; i < 2; i++ ){
		for( int j = 0; j < 2; j++ ){
	
			Ray ray2  = rayGenerator.generate( i, j );
			//ray2.printInfo();

			//point = getNearestIntersection_1( ray2, &iFacePtr );
			iFacePtr = getNearestIntersection( ray2, object->myFaceList, &point );

			if( iFacePtr != NULL ){
				printf("intersected point:");
				point.printInfo();
				printf("\n");
				iFacePtr->printInfo();
			}
		}
	}
	return 0;
#endif
}