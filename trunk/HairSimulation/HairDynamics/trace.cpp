#include	"trace.h"

//��X���쪺�I���C��
Vector3 computeColor( const Vector3 &point, const Vector3 &n, Face *iFace, mesh *iMesh, const Vector3 &inRayDir )
{
	Vector3 color(0, 0, 0);
	if( iFace != 0 ){//������face
		switch( iMesh->lightingProperty ){
			case GENERAL:
				color = directLightingColor( point, n, iFace, inRayDir );
				//���� indirectLighting:
				break;

			case MIRROR:
				//�����譱����
				//���ݬO���쥿���Τϭ�
				//�ϭ����κޤF
				float cosR = dotProduct( inRayDir , n);
				if( cosR < -EPSILON ){	//����T���Ϊ�����,���I���~,��F�ܤ[
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
//��X�Ϯg�����C��
Vector3 reflectedLightColor( const Vector3 &point, const Vector3 &n, const Vector3 &inRayDir )
{
	float cosR = dotProduct(inRayDir , n);
	if( cosR < -EPSILON ){ //����T���Ϊ�����,���I���~,��F�ܤ[
		Ray reflectRay;
		reflectRay.start = point;
		Vector3 inRay_n = cosR *n;	// incoming ray's component: inRay_n, inRay_t = inRayDir - inRay_n;
		//reflected ray's reRay = inRay_t - inRay_n
		reflectRay.dir = normalize( inRayDir - inRay_n - inRay_n );

		Vector3 hitPoint, hitNormal;
		mesh *hitMesh;
		Face* hitFace = getNearestIntersection_v4( reflectRay, objectList, 
			&hitPoint, &hitNormal, &hitMesh );

		//���j��X�U�@�ӥ��쪺face
		return computeColor( hitPoint, hitNormal, hitFace, hitMesh, reflectRay.dir );

	}else{
		return Vector3(0, 0, 0);
	}
}

Vector3 directLightingColor( const Vector3 &point, const Vector3 &n, Face *iFacePtr, const Vector3 &inRayDir )
{	
	Vector3 color( 0.0, 0.0, 0.0 );	//�w�]�¦�
	if( iFacePtr != NULL ){	//��intersect
		Ray lightRay;		//�ϦV���^���������u
		lightRay.start = point;

		mesh *hitMesh;	//�������mesh����
		Vector3 hitPoint;	//���쪺�I
		Vector3 hitNormal;	//�I��normal
		Vector3 d_L = Vector3( light.pos.ptr ) - point;
		lightRay.dir = normalize( d_L );

		//�P�_����������u�����O�_�|�b����t�@�ӤT����
		//���ݭn�ެO����T���Ϊ������άO�ϭ�,�ϭ��]�|�B���
		//�`�N:�A���������face���n�O�ۤv
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
		// n �O�o��point ������ normal
		float cos1 = dotProduct(n , lightRay.dir);
		if( cos1 > EPSILON ){
			for( int i = 0; i < 3; i++ ){
				color[i] += iFacePtr->materialPtr->Kd[i] * light.Ld[i] * cos1;
			}
			// specular
			Vector3 L_n = (dotProduct(d_L , n)) * n;//L�������q
			Vector3 R = L_n - ( d_L - L_n ); //�Ϯg��VR = L_n - L_t.	L_t = d_L - L_n
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
	glDisable(GL_DEPTH_TEST);	//�����`�״���
	glDisable( GL_LIGHTING );	//������
	// projection
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0.0, winW, 0.0, winH, -1.0, 1.0 );//or gluOrtho2D( 0.0, winW, 0.0, winH );
	// viewing, modeling
	glMatrixMode(GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( 0, 0, 1.0, 0, 0, 0.0, 0, 1, 0 );
	// �]�M���e���� color
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
			Vector3 point;		//���쪺�I
			Vector3 normal;	//���쪺�I��normal
			mesh *iMesh = 0;//���쪺mesh object
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
	if( iFace != 0 ){//������face
		switch( iMesh->lightingProperty ){
			case GENERAL:
				color = directLightingColor( point, n, iFace, inRayDir );
				//���� indirectLighting:
				//²�ƪ�-��Ϯg�����C��
				// one-bounce reflective lighting
				color = color + 0.1* reflectedLightColor( point, n, inRayDir );
				for( int i = 0; i < 3; i++ ){
					if( color[i] > 1.0f ){
						color[i] = 1.0f;
					}
				}
				break;

			case MIRROR:
				//�����譱����
				//���ݬO���쥿���Τϭ�
				//�ϭ����κޤF
				float cosR = dotProduct(inRayDir , n);
				if( cosR < -EPSILON ){	//����T���Ϊ�����,���I���~,��F�ܤ[
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
					//���λ��j�A�]���������u���@���譱����
					//���j��X�U�@�ӥ��쪺face
					//�]���໼�j
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
	gluPerspective( 86.0, (GLdouble) winW/winH, 0.1, 100.0 );//�۾����Y�Mimage plane�W�U�ݪ�������90
	// viewing, modeling
	glMatrixMode(GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( camera.eyePos.x, camera.eyePos.y, camera.eyePos.z,	// eyePos
				camera.center.x, camera.center.y, camera.center.z,	// center
				camera.up.x, camera.up.y, camera.up.z );			// up

	//setLight();	//�]�w����
	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );
	glClearColor( 0.0, 0.0, 0.0, 0.0 );	// �]�M���e���� color
	//glClearDepth(1.0f);					//depth buffer setup
	//glEnable( GL_DEPTH_TEST );			//enable depth test
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// �M�� buffer
	
	//draw every mesh-object
	for( std::vector< mesh* >::const_iterator mPtr = objectList.begin(); mPtr != objectList.end(); ++mPtr ){
		for( std::vector< Face >::const_iterator fPtr = (*mPtr)->myFaceList.begin(); fPtr != (*mPtr)->myFaceList.end(); ++fPtr ){

			glMaterialfv(GL_FRONT, GL_AMBIENT , fPtr-> materialPtr->Ka );
			glMaterialfv(GL_FRONT, GL_DIFFUSE ,	fPtr->materialPtr->Kd  );
			glMaterialfv(GL_FRONT, GL_SPECULAR,	fPtr->materialPtr->Ks );
			glMaterialf(GL_FRONT, GL_SHININESS,	fPtr->materialPtr->Ns );
			glColor3fv( fPtr->materialPtr->Kd );
			//�e��u�ι���
			//glBegin( GL_TRIANGLES );
			//glBegin( GL_LINE_LOOP );

			//for( int j = 0; j < 3; j++ ){
			//	glNormal3fv( fPtr->nPtr[j]->ptr );
			//	glVertex3fv( fPtr->vPtr[j]->ptr );
			//}	
			//glEnd();
		}
	}
	
	//debug ����u�e�X��
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
			Vector3 point;		//���쪺�I
			Vector3 normal;	//���쪺�I��normal
			mesh *iMesh = 0;//���쪺mesh object
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
