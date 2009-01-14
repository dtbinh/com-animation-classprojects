#ifndef _APP_SELECTION_H_
#define _APP_SELECTION_H_

#include "AppPrerequisites.h"
#include <OIS/OIS.h>
#include <CEGUI/CEGUI.h>

/*
	class MeshSelecion
	--------------------------------------------------
	The class contains the methods for mesh selection.
*/
class MeshSelection
{
public:
	MeshSelection(Camera* camera);
	void ButtonDown(const OIS::MouseEvent &mouseEvent, MeshPtr pMesh);
	void MouseMove( int MouseX, int MouseY );
	void ButtonUp( MeshPtr pMesh );
	void getFrustum( Vector3 Normals[4], Vector3 Points[8] );
	void getLine( Vector3 &P1, Vector3 &P2, int MouseX, int MouseY );
	void setWindowHeight( int Height );
	void render(void);

private:
	CEGUI::Point mStartPoint, mEndPoint;
	bool mButton;
	int mWindowHeight;
	Camera* mCamera;
};

#endif