#include "AppSelection.h"

//------------------------------------------------------------------------
MeshSelection::MeshSelection(Camera* camera) : mCamera(camera)
{}

//------------------------------------------------------------------------
void MeshSelection::ButtonDown(const OIS::MouseEvent &mouseEvent, MeshPtr pMesh )
{
	Ray mouseRay;

	mStartPoint = CEGUI::MouseCursor::getSingleton().getPosition();
	mEndPoint = mStartPoint;
	mButton = true;

	//	Create a ray from the selection
	mouseRay = mCamera->getCameraToViewportRay(mStartPoint.d_x/float(mouseEvent.state.width), mStartPoint.d_y/float(mouseEvent.state.height));

	// If control or alt isn't held, clear the selection

	// Alt unselects triangles

	// Select Triangle Clicked on
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
void MeshSelection::getLine( Vector3 &P1, Vector3 &P2, int MouseX, int MouseY )
{
}

//------------------------------------------------------------------------
void MeshSelection::setWindowHeight( int Height )
{
}

//------------------------------------------------------------------------
void MeshSelection::render(void)
{
}
