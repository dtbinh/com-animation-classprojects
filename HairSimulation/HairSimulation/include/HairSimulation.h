/*
-----------------------------------------------------------------------------
Filename:    HairSimulation.h
-----------------------------------------------------------------------------

This source file is generated by the Ogre AppWizard.

Check out: http://conglomerate.berlios.de/wiki/doku.php?id=ogrewizards

Based on the Example Framework for OGRE
(Object-oriented Graphics Rendering Engine)

Copyright (c) 2000-2007 The OGRE Team
For the latest info, see http://www.ogre3d.org/

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the OGRE engine.
-----------------------------------------------------------------------------
*/
#ifndef __HairSimulation_h_
#define __HairSimulation_h_


#include "ExampleApplication.h"
#include "World.h"
#include "ApplicationObject.h"
#include "AppSelection.h"
#include "DynamicLines.h"	//	line

#include <CEGUI/CEGUI.h>
#include <OgreCEGUIRenderer.h>
#include <iostream>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "../res/resource.h"
#endif

using namespace std;
using namespace OgreOpcode;


/** Convert OIS mouse buttons to CEGUI's */
CEGUI::MouseButton convertMouseButton(OIS::MouseButtonID buttonID)
{
    switch (buttonID)
    {
    case OIS::MB_Left:
        return CEGUI::LeftButton;

    case OIS::MB_Right:
        return CEGUI::RightButton;

    case OIS::MB_Middle:
        return CEGUI::MiddleButton;

    default:
        return CEGUI::LeftButton;
    }
}

class HairSimulationFrameListener : public ExampleFrameListener, public OIS::MouseListener
{
private:
   SceneManager* mSceneMgr;			//	A pointer to the scene manager
   World*		 mWorld;
   MeshSelection*	mMeshSelection;
   CMesh*			mMesh;


public:
	HairSimulationFrameListener(SceneManager *sceneMgr, RenderWindow* win, Camera* cam, World* world, CMesh* mesh)
         : ExampleFrameListener(win, cam, false, true), mSceneMgr(sceneMgr), mWorld(world), mMesh(mesh)
	{
		mMouse->setEventCallback(this);
		mMeshSelection = new MeshSelection(cam, mKeyboard);
	}

	//-----------------------------------------------------------------//
	~HairSimulationFrameListener()
	{
	}

	//-----------------------------------------------------------------//
	bool frameStarted(const FrameEvent& evt)
	{
		bool ret = ExampleFrameListener::frameStarted(evt);
		return ret;
	}

	//--------------MouseListener--------------------------------------------------//
	bool mouseMoved( const OIS::MouseEvent &arg )
	{
		// Rotation factors
		const OIS::MouseState &ms = arg.state;
		if( ms.buttonDown( OIS::MB_Right ) )
		{
			mRotX = Degree(-ms.X.rel * 0.13);
			mRotY = Degree(-ms.Y.rel * 0.13);
		}
		//	Do camera rotation
		mCamera->yaw(mRotX);
		mCamera->pitch(mRotY);

		CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
		return true;
	}

	//----------------------------------------------------------------//
	bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		CEGUI::System::getSingleton().injectMouseButtonDown(convertMouseButton(id));

		// Use RaySceneQuery to do scalp selection
		if (mWorld->getProcessState() == World::PS_SELECT_SCALP)
		{
			if (id == OIS::MB_Left)
			{
				mMesh->markBackfacing(mCamera->getPosition());
				mMeshSelection->ButtonDown(arg, mMesh);
				mMesh->renderSelectedMesh();
			}
		}

		return true;
	}

	//----------------------------------------------------------------//
	bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		CEGUI::System::getSingleton().injectMouseButtonUp(convertMouseButton(id));
		return true;
	}
	
};



class HairSimulationApp : public ExampleApplication
{
public:
	HairSimulationApp()
		: mGUISystem(0), mGUIRenderer(0)
	{}

	~HairSimulationApp()
	{
		//	Delete GUI system and renderer
		if (mGUISystem)
		{
			delete mGUISystem;
			mGUISystem = 0;
		}
		
		if (mGUIRenderer)
		{
			delete mGUIRenderer;
			mGUIRenderer = 0;
		}
	}

protected:
	World* mWorld;
	ApplicationObject* mHead;
	//	GUI system and renderer
	CEGUI::System *mGUISystem;
	CEGUI::OgreCEGUIRenderer * mGUIRenderer;
	//	GUI components
	CEGUI::Window *mButtonQuit, *mButton1, *mButton2, *mButton3, *mButtonTest, *mButton1Finish;
	

	virtual void createCamera(void)
	{
      // Create the camera
      mCamera = mSceneMgr->createCamera("PlayerCam");

      // Position it at 500 in Z direction
      mCamera->setPosition(Vector3(0,0,80));
      // Look back along -Z
      mCamera->lookAt(Vector3(0,0,-300));
      mCamera->setNearClipDistance(5);
	}


    virtual bool configure(void)
    {
        // Show the configuration dialog and initialise the system
        // You can skip this and use root.restoreConfig() to load configuration
        // settings if you were sure there are valid ones saved in ogre.cfg
		if(mRoot->showConfigDialog())
        {
            // If returned true, user clicked OK so initialise
            // Here we choose to let the system create a default rendering window by passing 'true'
            mWindow = mRoot->initialise(true);
			// Let's add a nice window icon
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			HWND hwnd;
			mWindow->getCustomAttribute("WINDOW", (void*)&hwnd);
			LONG iconID   = (LONG)LoadIcon( GetModuleHandle(0), MAKEINTRESOURCE(IDI_APPICON) );
			SetClassLong( hwnd, GCL_HICON, iconID );
#endif
            return true;
        }
        else
        {
            return false;
        }
    }


	// Just override the mandatory create scene method
	virtual void createScene(void)
	{
		//	Create the world
		mWorld = new World(mSceneMgr);
		
		//	[Error] The mesh info is not consistent with build-in mesh(Scale is different)
		/*
		//	Create a sphere
		mHead = mWorld->createBall("ball", 7);
		mHead->getEntity()->setMaterialName("");	// Color white
*/
		
		//	Creaete an ogre head
		mHead = mWorld->createOgreHead("OgreHead");
		
		
		//	Creaete a man head
		//mHead = mWorld->createManHead("ManHead");


		// Set ambient light
		mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

		// Create a light
		Light* l = mSceneMgr->createLight("MainLight");
		l->setPosition(20,80,50);

		//	Setup GUI system
		mGUIRenderer = new CEGUI::OgreCEGUIRenderer(mWindow, Ogre::RENDER_QUEUE_OVERLAY, false, 3000, mSceneMgr);
		mGUISystem = new CEGUI::System(mGUIRenderer);

		// load scheme and set up defaults
		CEGUI::SchemeManager::getSingleton().loadScheme((CEGUI::utf8*)"TaharezLookSkin.scheme");
		mGUISystem->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
		mGUISystem->setDefaultFont((CEGUI::utf8*)"BlueHighway-12");

		CEGUI::WindowManager *win = CEGUI::WindowManager::getSingletonPtr();
       
		CEGUI::Window *sheet = win->createWindow("DefaultGUISheet", "HairSimulationApp/Sheet");

		createButtons(win, sheet);
		mGUISystem->setGUISheet(sheet);
	}

   // Create new frame listener
	void createFrameListener(void)
	{
      mFrameListener= new HairSimulationFrameListener(mSceneMgr, mWindow, mCamera, mWorld, mHead->getMesh());
		mRoot->addFrameListener(mFrameListener);
	}

	/** Create buttons */
	void createButtons(CEGUI::WindowManager *win, CEGUI::Window *sheet)
	{
		
		mButtonQuit = win->createWindow("TaharezLook/Button", "HairSimulationApp/ButtonQuit");
		mButton1 = win->createWindow("TaharezLook/Button", "HairSimulationApp/Button1");
		mButton2 = win->createWindow("TaharezLook/Button", "HairSimulationApp/Button2");
		mButton3 = win->createWindow("TaharezLook/Button", "HairSimulationApp/Button3");
		mButtonTest = win->createWindow("TaharezLook/Button", "HairSimulationApp/ButtonTest");
		mButton1Finish = win->createWindow("TaharezLook/Button", "HairSimulationApp/Button1Finish");

		//	Button1
		mButton1->setText("1. Select scalp range");
		mButton1->setSize(CEGUI::UVector2(CEGUI::UDim(0.20, 0), CEGUI::UDim(0.05, 0)));
		mButton1->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
		mButton1->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber(&HairSimulationApp::handleButton1, this));
		sheet->addChildWindow(mButton1);

		//	Button2
		mButton2->setText("2. Generate hairs");
		mButton2->setSize(CEGUI::UVector2(CEGUI::UDim(0.20, 0), CEGUI::UDim(0.05, 0)));
		mButton2->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0.10, 0)));
		mButton2->setVisible(false);
		mButton2->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber(&HairSimulationApp::handleButton2, this));
		sheet->addChildWindow(mButton2);

		//	Button3
		mButton3->setText("3. Simulate dynamics");
		mButton3->setSize(CEGUI::UVector2(CEGUI::UDim(0.20, 0), CEGUI::UDim(0.05, 0)));
		mButton3->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0.20, 0)));
		mButton3->setVisible(false);
		mButton3->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber(&HairSimulationApp::handleButton3, this));
		sheet->addChildWindow(mButton3);

		//	Button quit
		mButtonQuit->setText("Quit");
		mButtonQuit->setSize(CEGUI::UVector2(CEGUI::UDim(0.20, 0), CEGUI::UDim(0.05, 0)));
		mButtonQuit->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0.3, 0)));

		mButtonQuit->subscribeEvent(CEGUI::PushButton::EventClicked,
		   CEGUI::Event::Subscriber(&HairSimulationApp::quit, this));
		sheet->addChildWindow(mButtonQuit);

		//	ButtonTest
		mButtonTest->setText("Test");
		mButtonTest->setSize(CEGUI::UVector2(CEGUI::UDim(0.20, 0), CEGUI::UDim(0.05, 0)));
		mButtonTest->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0.40, 0)));
		//mButtonTest->setVisible(false);
		mButtonTest->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber(&HairSimulationApp::handleButtonTest, this));
		sheet->addChildWindow(mButtonTest);

		//	Button1Finish
		mButton1Finish->setText("Finish");
		mButton1Finish->setSize(CEGUI::UVector2(CEGUI::UDim(0.20, 0), CEGUI::UDim(0.05, 0)));
		mButton1Finish->setPosition(CEGUI::UVector2(CEGUI::UDim(0.2, 0), CEGUI::UDim(0, 0)));
		mButton1Finish->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber(&HairSimulationApp::handleButton1Finish, this));
		sheet->addChildWindow(mButton1Finish);
       
	}

	//----------------------Handler functions for button events--------
	bool handleButton1(const CEGUI::EventArgs &e)
	{
		mWorld->setProcessState(World::PS_SELECT_SCALP);
		mButton1->setVisible(false);
		mButton1Finish->setVisible(true);

		return true;
	}
	//-----------------------------------------------------------------
	bool handleButton2(const CEGUI::EventArgs &e)
	{
		mWorld->setProcessState(World::PS_GENERATION);
		mButton2->setVisible(false);
		mButton3->setVisible(true);

		return true;
	}
	//-----------------------------------------------------------------
	bool handleButton3(const CEGUI::EventArgs &e)
	{
		mWorld->setProcessState(World::PS_SIMULATION);
		mButton3->setVisible(false);

		return true;
	}
	//------------------ Handle Quit button --------------------------
	bool quit(const CEGUI::EventArgs &e)
    {
        exit(0);
        return true;
    }
	//------------------ Handle Test button --------------------------
	bool handleButtonTest(const CEGUI::EventArgs &e)
	{
		cout << "Button test is pressed" << endl;
		return true;
	}
	//------------------ Handle Button1Finish ------------------------
	bool handleButton1Finish(const CEGUI::EventArgs &e)
	{


		mButton1Finish->setVisible(false);
		mButton2->setVisible(true);
		return true;
	}
	
};

#endif // #ifndef __HairSimulation_h_