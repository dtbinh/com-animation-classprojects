#include	"trace.h"

//算出打到的點的顏色
Vector3 computeColor( const Vector3 &point, const Vector3 &n, Face *iFace, mesh *iMesh, const Vector3 &inRayDir )
{
	Vector3 color(0, 0, 0);
	if( iFace != 0 ){//有打到face
		switch( iMesh->lightingProperty ){
			case GENERAL:
				color = directLightingColor( point, n, iFace, inRayDir );
				//不做 indirectLighting:
				break;

			case MIRROR:
				//打到鏡面的話
				//先看是打到正面或反面
				//反面不用管了
				float cosR = dotProduct( inRayDir , n);
				if( cosR < -EPSILON ){	//打到三角形的正面,笑點錯誤,找了很久
					Ray reflectRay;
					reflectRay.start = point;
					Vector3 inRay_n = cosR *n;	// incoming ray's component: inRay_n, inRay_t
					Vector3 inRay_t = inRayDir - inRay_n;
					//reflected ray's reRay = inRay_t - inRay_n
					reflectRay.dir = normalize( inRay_t - inRay_n );

					Vector3 hitPoint, hitNormal;
					mesh *hitMesh;
					Face* hitFace = getNearestIntersection_v4( reflectRay, objectList, 
								&hitPoint, &hitNormal, &hitMesh );

					color = directLightingColor( hitPoint, hitNormal, hitFace, reflectRay.dir );
				}
				break;
		}
	}
	return color;
}
//找出反射光的顏色
Vector3 reflectedLightColor( const Vector3 &point, const Vector3 &n, const Vector3 &inRayDir )
{
	float cosR = dotProduct(inRayDir , n);
	if( cosR < -EPSILON ){ //打到三角形的正面,笑點錯誤,找了很久
		Ray reflectRay;
		reflectRay.start = point;
		Vector3 inRay_n = cosR *n;	// incoming ray's component: inRay_n, inRay_t = inRayDir - inRay_n;
		//reflected ray's reRay = inRay_t - inRay_n
		reflectRay.dir = normalize( inRayDir - inRay_n - inRay_n );

		Vector3 hitPoint, hitNormal;
		mesh *hitMesh;
		Face* hitFace = getNearestIntersection_v4( reflectRay, objectList, 
			&hitPoint, &hitNormal, &hitMesh );

		//遞迴找出下一個打到的face
		return computeColor( hitPoint, hitNormal, hitFace, hitMesh, reflectRay.dir );

	}else{
		return Vector3(0, 0, 0);
	}
}

Vector3 directLightingColor( const Vector3 &point, const Vector3 &n, Face *iFacePtr, const Vector3 &inRayDir )
{	
	Vector3 color( 0.0, 0.0, 0.0 );	//預設黑色
	if( iFacePtr != NULL ){	//有intersect
		Ray lightRay;		//反向打回光源的光線
		lightRay.start = point;

		mesh *hitMesh;	//打到哪個mesh物體
		Vector3 hitPoint;	//打到的點
		Vector3 hitNormal;	//點的normal
		Vector3 d_L = Vector3( light.pos.ptr ) - point;
		lightRay.dir = normalize( d_L );

		//判斷到光源的光線中間是否會在打到另一個三角形
		//不需要管是打到三角形的正面或是反面,反面也會遮住光
		//注意:再次找到交錯的face不要是自己
		Face *iFace = 
			getNearestIntersection_v4( lightRay, objectList, &hitPoint, &hitNormal, &hitMesh );
		if( iFace != 0 ){
			Vector3 d_hit = hitPoint - point;
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
		// n 是這個point 內插的 normal
		float cos1 = dotProduct(n , lightRay.dir);
		if( cos1 > EPSILON ){
			for( int i = 0; i < 3; i++ ){
				color[i] += iFacePtr->materialPtr->Kd[i] * light.Ld[i] * cos1;
			}
			// specular
			Vector3 L_n = (dotProduct(d_L , n)) * n;//L垂直分量
			Vector3 R = L_n - ( d_L - L_n ); //反射方向R = L_n - L_t.	L_t = d_L - L_n
			R.normalize();
			float cos2 = dotProduct((-R) , inRayDir);
			if( cos2 > EPSILON ){
				for( int i = 0; i < 3; i++ ){
					color[i] += iFacePtr->materialPtr->Ks[i] * light.Ls[i] * pow( cos2, iFacePtr->materialPtr->Ns );
				}
			}
		}
		
	}
	return color;
}




void display1()
{
	glDisable(GL_DEPTH_TEST);	//關掉深度測試
	glDisable( GL_LIGHTING );	//不打光
	// projection
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0.0, winW, 0.0, winH, -1.0, 1.0 );//or gluOrtho2D( 0.0, winW, 0.0, winH );
	// viewing, modeling
	glMatrixMode(GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( 0, 0, 1.0, 0, 0, 0.0, 0, 1, 0 );
	// 設清除畫布的 color
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClear( GL_COLOR_BUFFER_BIT );
	
	float near_scalar = 1.0, right = 1.0, top = 1.0;	//image plane info
	Vector3 eyeDir = camera.center - camera.eyePos;
	RayGenerator rayGen( winW, winH, right, top, near_scalar );
	rayGen.setEyeInfo( &(camera.eyeDir), &camera.eyePos, &camera.up );
//#define DEBUG
#ifdef DEBUG
	FILE *fp;
	fp = fopen( "debug.txt", "w+" );
#endif
	for( int i = 0; i < winW; i++ ){
		for( int j = 0; j < winH; j++ ){
			Ray ray = rayGen.generate( i, j );
			Vector3 point;		//打到的點
			Vector3 normal;	//打到的點的normal
			mesh *iMesh = 0;//打到的mesh object
			Face *iFacePtr = 
				getNearestIntersection_v4(ray, objectList, &point, &normal, &iMesh );
			
			Vector3 pixelColor = computeColor( point, normal, iFacePtr, iMesh, ray.dir );
			
#ifdef DEBUG
			pixelColor.printInfo( fp );
#endif
			glBegin( GL_POINTS );
			glColor3fv( pixelColor.ptr );
			glVertex3i( i, j, 0 );
			glEnd();
		}
#ifdef DEBUG
		fprintf(fp, "\n");
#endif
	}
	
	printf("finish\n");
	glFlush();
}

Vector3 computeColor_debug( const Vector3 &point, const Vector3 &n, Face *iFace, mesh *iMesh, const Vector3 &inRayDir )
{
	Vector3 color(0, 0, 0);
	if( iFace != 0 ){//有打到face
		switch( iMesh->lightingProperty ){
			case GENERAL:
				color = directLightingColor( point, n, iFace, inRayDir );
				//接著 indirectLighting:
				//簡化版-找反射光的顏色
				// one-bounce reflective lighting
				color = color + 0.1* reflectedLightColor( point, n, inRayDir );
				for( int i = 0; i < 3; i++ ){
					if( color[i] > 1.0f ){
						color[i] = 1.0f;
					}
				}
				break;

			case MIRROR:
				//打到鏡面的話
				//先看是打到正面或反面
				//反面不用管了
				float cosR = dotProduct(inRayDir , n);
				if( cosR < -EPSILON ){	//打到三角形的正面,笑點錯誤,找了很久
					Ray reflectRay;
					reflectRay.start = point;
					Vector3 inRay_n = cosR *n;	// incoming ray's component: inRay_n, inRay_t
					Vector3 inRay_t = inRayDir - inRay_n;
					//reflected ray's reRay = inRay_t - inRay_n
					reflectRay.dir = normalize( inRay_t - inRay_n );
					
#ifdef DEBUG_12_12_0
					//Vec3 reRayGo = point + ( reflectRay.dir * 5.0 );
						//glColor3f( 1.0, 0.0, 0.7 );
						//glBegin( GL_LINES );
						//glVertex3fv(point.ptr);
						//glVertex3fv(reRayGo.ptr);
						//glEnd();
#endif

					Vector3 hitPoint, hitNormal;
					mesh *hitMesh;
					Face* hitFace = getNearestIntersection_v4( reflectRay, objectList, 
								&hitPoint, &hitNormal, &hitMesh );
//#define DEBUG_12_12
//#ifdef DEBUG_12_12
					if( hitFace ){
						glColor3fv( hitFace->materialPtr->Kd ); //glColor3f( 1.0, 0.0, 0.7 );
						glBegin( GL_LINES );
						glVertex3fv(point.ptr);
						glVertex3fv(hitPoint.ptr);
						glEnd();
					}
//#endif
					color = directLightingColor( hitPoint, hitNormal, hitFace, reflectRay.dir );
					//不用遞迴，因為場景中只有一個鏡面物體
					//遞迴找出下一個打到的face
					//也不能遞迴
					//color = computeColor( hitPoint, hitNormal, hitFace, hitMesh, reflectRay.dir );
					
				}
				break;
		}
	}
	return color;
}

void display_debug()
{
	// projection
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 86.0, (GLdouble) winW/winH, 0.1, 100.0 );//相機鏡頭和image plane上下端的夾角約90
	// viewing, modeling
	glMatrixMode(GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( camera.eyePos.x, camera.eyePos.y, camera.eyePos.z,	// eyePos
				camera.center.x, camera.center.y, camera.center.z,	// center
				camera.up.x, camera.up.y, camera.up.z );			// up

	//setLight();	//設定光源
	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );
	glClearColor( 0.0, 0.0, 0.0, 0.0 );	// 設清除畫布的 color
	//glClearDepth(1.0f);					//depth buffer setup
	//glEnable( GL_DEPTH_TEST );			//enable depth test
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// 清除 buffer
	
	//draw every mesh-object
	for( std::vector< mesh* >::const_iterator mPtr = objectList.begin(); mPtr != objectList.end(); ++mPtr ){
		for( std::vector< Face >::const_iterator fPtr = (*mPtr)->myFaceList.begin(); fPtr != (*mPtr)->myFaceList.end(); ++fPtr ){

			glMaterialfv(GL_FRONT, GL_AMBIENT , fPtr-> materialPtr->Ka );
			glMaterialfv(GL_FRONT, GL_DIFFUSE ,	fPtr->materialPtr->Kd  );
			glMaterialfv(GL_FRONT, GL_SPECULAR,	fPtr->materialPtr->Ks );
			glMaterialf(GL_FRONT, GL_SHININESS,	fPtr->materialPtr->Ns );
			glColor3fv( fPtr->materialPtr->Kd );
			//畫格線或實體
			//glBegin( GL_TRIANGLES );
			//glBegin( GL_LINE_LOOP );

			//for( int j = 0; j < 3; j++ ){
			//	glNormal3fv( fPtr->nPtr[j]->ptr );
			//	glVertex3fv( fPtr->vPtr[j]->ptr );
			//}	
			//glEnd();
		}
	}
	
	//debug 把光線畫出來
	float near_scalar = 1.0, right = 1.0, top = 1.0;	//image plane info
	
	RayGenerator rayGen( winW, winH, right, top, near_scalar );
	rayGen.setEyeInfo( &(camera.eyeDir), &camera.eyePos, &camera.up );
//#define DEBUG
#ifdef DEBUG
	FILE *fp;
	fp = fopen( "debug.txt", "w+" );
#endif
	
	for( int i = 0; i < winW; i++ ){
		for( int j = 0; j < winH; j++ ){
			Ray ray = rayGen.generate( i, j );
			Vector3 point;		//打到的點
			Vector3 normal;	//打到的點的normal
			mesh *iMesh = 0;//打到的mesh object
			Face *iFacePtr = 
				getNearestIntersection_v4(ray, objectList, &point, &normal, &iMesh );
			if( iFacePtr != 0 ){//hit
				
				//glColor3fv( iFacePtr->materialPtr->Kd );
				glColor3f(1.0, 1.0, 1.0);
				//glBegin( GL_TRIANGLES );
				glBegin( GL_LINE_LOOP );
				//glBegin( GL_LINE_STRIP );
				glVertex3fv( iFacePtr->vPtr[0]->ptr );
				glVertex3fv( iFacePtr->vPtr[1]->ptr );
				glVertex3fv( iFacePtr->vPtr[2]->ptr );
					//glVertex3fv(ray.start.ptr);
					//glVertex3fv(point.ptr);
				glEnd();
				
				//if( iMesh->lightingProperty == MIRROR ){
				//glPushMatrix();
				//glTranslatef( point.x, point.y, point.z );
				//glutSolidSphere( 1.0, 20, 20 );
				//glPopMatrix();
				//}
				glColor3f( 1.0, 0.0, 0.7 );
				glBegin( GL_LINES );
					glVertex3fv(ray.start.ptr);
					glVertex3fv(point.ptr);
				glEnd();
			}
			
			Vector3 pixelColor = computeColor_debug( point, normal, iFacePtr, iMesh, ray.dir );
			
			//glColor3fv( pixelColor.ptr );
			//glVertex3i( i, j, 0 );
		}

	}
	printf("finish\n");
	glFlush();
}
