#include "AppSelection.h"

//------------------------------------------------------------------------
MeshSelection::MeshSelection(Camera* camera, OIS::Keyboard* kb) : mCamera(camera), mKeyboard(kb)
{}

//------------------------------------------------------------------------
void MeshSelection::ButtonDown(const OIS::MouseEvent &mouseEvent, CMesh* pMesh )
{
	

	mStartPoint = CEGUI::MouseCursor::getSingleton().getPosition();
	mEndPoint = mStartPoint;
	mButton = true;

	// Create a line from the selection
	getLine(mLine[0], mLine[1], mStartPoint.d_x/float(mouseEvent.state.width), mStartPoint.d_y/float(mouseEvent.state.height));

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
void MeshSelection::MouseMove( int MouseX, int MouseY )
{
}

//------------------------------------------------------------------------
void MeshSelection::ButtonUp( MeshPtr pMesh )
{
}

//------------------------------------------------------------------------
void MeshSelection::getFrustum( Vector3 Normals[4], Vector3 Points[8] )
{
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
}

//------------------------------------------------------------------------
void MeshSelection::render(void)
{
}
