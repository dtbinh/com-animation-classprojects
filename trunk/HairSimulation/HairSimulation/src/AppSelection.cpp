#include "AppSelection.h"

void inline swap( float &x, float &y )
{
 float temp = x; 
 x = y;
 y = temp;
}

//------------------------------------------------------------------------
MeshSelection::MeshSelection(Camera* camera, OIS::Keyboard* kb) : mCamera(camera), mKeyboard(kb)
{}

//------------------------------------------------------------------------
void MeshSelection::ButtonDown(const OIS::MouseEvent &mouseEvent, CMesh* pMesh )
{
	CEGUI::Point cursorPos;

	cursorPos = CEGUI::MouseCursor::getSingleton().getPosition();
	mStartX = cursorPos.d_x;
	mStartY = cursorPos.d_y;
	mEndX = mStartX;
	mEndY = mStartY;
	mButton = true;

	// Create a line from the selection
	getLine(mLine[0], mLine[1], mStartX/float(mouseEvent.state.width), mStartY/float(mouseEvent.state.height));

	// If control or alt isn't held, clear the selection
	if (!mKeyboard->isKeyDown(OIS::KC_LCONTROL) &&
		!mKeyboard->isKeyDown(OIS::KC_RCONTROL) &&
		!mKeyboard->isKeyDown(OIS::KC_LMENU)	&&
		!mKeyboard->isKeyDown(OIS::KC_RMENU))
		pMesh->clearSelection();

	// Alt unselects triangles
	if (mKeyboard->isKeyDown(OIS::KC_LMENU) ||
		mKeyboard->isKeyDown(OIS::KC_RMENU))
		pMesh->setSelectionMode(CMesh::SELECT_SUB);
	else
		pMesh->setSelectionMode(CMesh::SELECT_ADD);

	// Select Triangle Clicked on
	pMesh->LineSelect(mLine[0], mLine[1]);
}

//------------------------------------------------------------------------
void MeshSelection::MouseMove( float MouseX, float MouseY )
{
	if(mButton)
	{
		if (MouseX < 32000.0f)
			mEndX = MouseX;
		else
			mEndX = 0.0f;

		if (MouseY < 32000.0f)
			mEndY = MouseY;
		else
			mEndY = 0.0f;
	}
}

//------------------------------------------------------------------------
/** Area slection on button up if drag area is large enough */
void MeshSelection::ButtonUp( CMesh* pMesh )
{
	mButton = false;
	if ( abs( mEndX- mStartX ) < 4 &&
		 abs( mEndY - mStartY ) < 4 ) 
		 return;
	if ( mEndX < mStartX ) swap( mEndX, mStartX );
	if ( mEndY < mStartY ) swap( mEndY, mStartY );
	
	Vector3 P[8];
	Vector3 Normals[6];
	getFrustum( Normals, P );
	pMesh->FrustumSelect( Normals, P );
}

//------------------------------------------------------------------------
void MeshSelection::getFrustum( Vector3 Normals[4], Vector3 P[8] )
{
	getLine( P[0], P[1], mStartX, mStartY );
	getLine( P[2], P[3], mStartX, mEndY );
	getLine( P[4], P[5], mEndX, mEndY );
	getLine( P[6], P[7], mEndX, mStartY );
	Normals[0] = (P[0]-P[1]).crossProduct( P[2]-P[3] );
	Normals[1] = (P[2]-P[3]).crossProduct( P[4]-P[5] );
	Normals[2] = (P[4]-P[5]).crossProduct( P[6]-P[7] );
	Normals[3] = (P[6]-P[7]).crossProduct( P[0]-P[1] );
	for (int i = 0; i < 4; i++) Normals[i].normalise();
}

//------------------------------------------------------------------------
void MeshSelection::getLine( Vector3 &P1, Vector3 &P2, float MouseX, float MouseY )
{
	Ray mouseRay;

	//	Create a ray from camera to  viewport
	mouseRay = mCamera->getCameraToViewportRay(MouseX, MouseY);

	mLine[0] = mouseRay.getOrigin();
	mLine[1] = mouseRay.getPoint(mCamera->getFarClipDistance());
}

//------------------------------------------------------------------------
void MeshSelection::setWindowHeight( int Height )
{
	mWindowHeight = Height;
}

//------------------------------------------------------------------------
void MeshSelection::render(void)
{
}
