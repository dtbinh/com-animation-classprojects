#ifndef _APP_SELECTION_H_
#define _APP_SELECTION_H_

#include "AppPrerequisites.h"
#include "Mesh.h"
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
	MeshSelection(Camera* camera, OIS::Keyboard* kb);
	void ButtonDown(const OIS::MouseEvent &mouseEvent, CMesh *pMesh);
	void MouseMove( float MouseX, float MouseY );
	void ButtonUp( CMesh* pMesh );
	void getFrustum( Vector3 Normals[4], Vector3 Points[8] );
	void getLine( Vector3 &P1, Vector3 &P2, float MouseX, float MouseY );
	void setWindowHeight( int Height );
	void render(void);

private:
	float mStartX, mStartY, mEndX, mEndY;
	float mWinWidth, mWinHeight;
	bool mButton;
	int mWindowHeight;
	Vector3 mLine[2];
	Camera *mCamera;
	OIS::Keyboard *mKeyboard;
	ManualObject	*mSelectionBox;
};

#endif