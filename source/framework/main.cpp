//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//-----------------------------------------------------------------
// Includes
//-----------------------------------------------------------------
//Application

//#define UseJPSPlus

#ifdef UseJPSPlus
#include "projects/App_PathfindingJPSPlus/App_PathfindingJPSPlus.h"
#else
#include "projects/App_PathfindingAStar/App_PathfindingAStar.h"
#include "projects/App_NavMeshGraph/App_NavMeshGraph.h"
#endif


//Hotfix for genetic algorithms project
bool gRequestShutdown = false;

//Main
#undef main //Undefine SDL_main as main
int main(int argc, char* argv[])
{
	srand(unsigned int(time(nullptr)));

	int x{}, y{};
	bool runExeWithCoordinates{ argc == 3 };

	if (runExeWithCoordinates)
	{
		x = stoi(string(argv[1]));
		y = stoi(string(argv[2]));
	}

	try
	{
		//Window Creation
		Elite::WindowParams params;
		EliteWindow* pWindow = new EliteWindow();
		ELITE_ASSERT(pWindow, "Window has not been created.");
		pWindow->CreateEWindow(params);

		if (runExeWithCoordinates)
			pWindow->SetWindowPosition(x, y);

		//Create Frame (can later be extended by creating FrameManager for MultiThreaded Rendering)
		EliteFrame* pFrame = new EliteFrame();
		ELITE_ASSERT(pFrame, "Frame has not been created.");
		pFrame->CreateFrame(pWindow);

		//Create a 2D Camera for debug rendering in this case
		Camera2D* pCamera = new Camera2D(params.width, params.height);
		ELITE_ASSERT(pCamera, "Camera has not been created.");
		DEBUGRENDERER2D->Initialize(pCamera);

		//Create Immediate UI 
		Elite::EImmediateUI* pImmediateUI = new Elite::EImmediateUI();
		ELITE_ASSERT(pImmediateUI, "ImmediateUI has not been created.");
		pImmediateUI->Initialize(pWindow->GetRawWindowHandle());

		//Create Physics
		PHYSICSWORLD; //Boot

		//Start Timer
		TIMER->Start();

		//Application Creation
		IApp* myApp = nullptr;

#ifdef UseJPSPlus
		myApp = new App_PathfindingJPSPlus();
#else
		//myApp = new App_PathfindingAStar();
		myApp = new App_NavMeshGraph();
#endif

		ELITE_ASSERT(myApp, "Application has not been created.");

		//Boot application
		myApp->Start();

		//Application Loop
		while (!pWindow->ShutdownRequested())
		{
			//Timer
			TIMER->Update();
			auto const elapsed = TIMER->GetElapsed();

			//Window procedure first, to capture all events and input received by the window
			if (!pImmediateUI->FocussedOnUI())
				pWindow->ProcedureEWindow();
			else
				pImmediateUI->EventProcessing();

			//New frame Immediate UI (Flush)
			pImmediateUI->NewFrame(pWindow->GetRawWindowHandle(), elapsed);

			//Update (Physics, App)
			PHYSICSWORLD->Simulate(elapsed);
			pCamera->Update();
			myApp->Update(elapsed);

			//Render and Present Frame
			PHYSICSWORLD->RenderDebug();
			myApp->Render(elapsed);
			pFrame->SubmitAndFlipFrame(pImmediateUI);
		}

		//Reversed Deletion
		SAFE_DELETE(myApp);
		SAFE_DELETE(pImmediateUI);
		SAFE_DELETE(pCamera);
		SAFE_DELETE(pFrame);
		SAFE_DELETE(pWindow);

		//Shutdown All Singletons
		PHYSICSWORLD->Destroy();
		DEBUGRENDERER2D->Destroy();
		INPUTMANAGER->Destroy();
		TIMER->Destroy();
	}
	catch (const Elite_Exception& e)
	{
		std::cout << e._msg << " Error: " << std::endl;
#ifdef PLATFORM_WINDOWS
		system("pause");
#endif
		return 1;
	}

	return 0;
}