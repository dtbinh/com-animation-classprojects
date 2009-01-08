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
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "../res/resource.h"
#endif

class HairSimulationFrameListener : public ExampleFrameListener
{
private:
   SceneManager* mSceneMgr;
public:
      HairSimulationFrameListener(SceneManager *sceneMgr, RenderWindow* win, Camera* cam)
         : ExampleFrameListener(win, cam),
         mSceneMgr(sceneMgr)
	{
	}

	bool frameStarted(const FrameEvent& evt)
	{
		bool ret = ExampleFrameListener::frameStarted(evt);

		return ret;

	}

};



class HairSimulationApp : public ExampleApplication
{
public:
	HairSimulationApp()
	{}

	~HairSimulationApp()
	{
	}

protected:
	World* mWorld;
	ApplicationObject* mHead;

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

		//	Create a sphere
		mHead = mWorld->createBall("ball", 7);
		mHead->getEntity()->setMaterialName("Ogre/Eyes");

		/*
		//	Creaete an ogre head
		mHead = mWorld->createOgreHead("OgreHead");
		*/

      // Set ambient light
      mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

      // Create a light
      Light* l = mSceneMgr->createLight("MainLight");
      l->setPosition(20,80,50);
	}

   // Create new frame listener
	void createFrameListener(void)
	{
      mFrameListener= new HairSimulationFrameListener(mSceneMgr, mWindow, mCamera);
		mRoot->addFrameListener(mFrameListener);
	}
};

#endif // #ifndef __HairSimulation_h_