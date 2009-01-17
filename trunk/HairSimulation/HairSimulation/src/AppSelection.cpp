#include "AppSelection.h"
#include "World.h"

void inline swapFloat( float &x, float &y )
{
 float temp = x; 
 x = y;
 y = temp;
}

//------------------------------------------------------------------------
MeshSelection::MeshSelection(Camera* camera, OIS::Keyboard* kb) : mCamera(camera), mKeyboard(kb)
{
	//	Setup the selection box
	mSelectionBox = World::getSingleton().getSceneManager()->createManualObject("SelectionBox");
	// Use identity view/projection matrices
	mSelectionBox->setUseIdentityProjection(true);
	mSelectionBox->setUseIdentityView(true);

	// Use infinite AAB to always stay visible
	AxisAlignedBox aabInf;
	aabInf.setInfinite();
	mSelectionBox->setBoundingBox(aabInf);
	
	// Render just before overlays
	mSelectionBox->setRenderQueueGroup(RENDER_QUEUE_OVERLAY - 1);
	
	// Attach to scene
	World::getSingleton().getSceneManager()->getRootSceneNode()->createChildSceneNode()->attachObject(mSelectionBox);
}

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
	mWinWidth = float(mouseEvent.state.width);
	mWinHeight = float(mouseEvent.state.height);

	// Create a line from the selection
	getLine(mLine[0], mLine[1], mStartX/mWinWidth, mStartY/mWinHeight);

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
/** Area selection on button up if drag area is large enough */
void MeshSelection::ButtonUp( CMesh* pMesh )
{
	mButton = false;
	if ( abs( mEndX- mStartX ) < 4 &&
		 abs( mEndY - mStartY ) < 4 ) 
		 return;
	if ( mEndX < mStartX ) swapFloat( mEndX, mStartX );
	if ( mEndY < mStartY ) swapFloat( mEndY, mStartY );

	Vector3 P[8];
	Vector3 Normals[6];
	getFrustum( Normals, P );
	pMesh->FrustumSelect( Normals, P );
}

//------------------------------------------------------------------------
void MeshSelection::getFrustum( Vector3 Normals[4], Vector3 P[8] )
{
	getLine( P[0], P[1], mStartX/mWinWidth, mStartY/mWinHeight );
	getLine( P[2], P[3], mStartX/mWinWidth, mEndY/mWinHeight );
	getLine( P[4], P[5], mEndX/mWinWidth, mEndY/mWinHeight );
	getLine( P[6], P[7], mEndX/mWinWidth, mStartY/mWinHeight );
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

	P1 = mouseRay.getOrigin();
	P2 = mouseRay.getPoint(mCamera->getFarClipDistance());
}

//------------------------------------------------------------------------
void MeshSelection::setWindowHeight( int Height )
{
	mWindowHeight = Height;
}

//------------------------------------------------------------------------
void MeshSelection::render(void)
{
	Real startX, startY, endX, endY;	//	shifted coordinates

	startX = mStartX/mWinWidth*2 - 1;
	startY = (mWinHeight- mStartY)/mWinHeight*2 - 1;
	endX = mEndX/mWinWidth*2 - 1;
	endY = (mWinHeight - mEndY)/mWinHeight*2 - 1;

	mSelectionBox->clear();
	if (mButton)
	{
		mSelectionBox->begin("BaseWhiteNoLighting", RenderOperation::OT_LINE_STRIP);

		mSelectionBox->position(startX, endY, 0.0);
		mSelectionBox->position( endX, endY, 0.0);
		mSelectionBox->position( endX,  startY, 0.0);
		mSelectionBox->position(startX,  startY, 0.0);

		mSelectionBox->index(0);
		mSelectionBox->index(1);
		mSelectionBox->index(2);
		mSelectionBox->index(3);
		mSelectionBox->index(0);
	
		mSelectionBox->end();
	}

}
