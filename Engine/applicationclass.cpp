////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
	m_Input = 0;
	m_Direct3D = 0;
	m_Camera = 0;
	m_Timer = 0;
	m_Position = 0;
	m_Fps = 0;
	m_Cpu = 0;

	m_Text = 0;

	m_Terrain = 0;
	m_QuadTree = 0;

	m_Light = 0;
	for (int i = 0; i < NUM_LIGHTS; i++)
		m_PointLights[i] = 0;
	m_gameManager = 0;
	m_renderManager = 0;


	m_freeCam = false;



}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	bool result;
	float cameraX, cameraY, cameraZ;
	D3DXMATRIX baseViewMatrix;
	char videoCard[128];
	int videoMemory;

	
	// Create the input object.  The input object will be used to handle reading the keyboard and mouse input from the user.
	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the Direct3D object.
	m_Direct3D = new D3DClass;
	if(!m_Direct3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// Set the initial position of the camera.
	cameraX = 50.0f;
	cameraY = 36.0f;
	cameraZ = -7.0f;
	float cameraRoll = 55.0f;
	m_Camera->SetPosition(cameraX, cameraY, cameraZ);
	m_Camera->SetRotation(cameraRoll,0.0f,0.0f);

	// Create the terrain object.
	m_Terrain = new TerrainClass;
	if(!m_Terrain)
	{
		return false;
	}

	// Initialize the terrain object.
		//result = m_Terrain->Initialize(m_Direct3D->GetDevice(), "../Engine/data/heightmap01.bmp",L".. / Engine / data / dirt01.dds");
	result = m_Terrain->InitializeTerrain(m_Direct3D->GetDevice(), 256,256, L"../Engine/data/dirt01.dds", L"../Engine/data/cracks.dds",
		L"../Engine/data/rock.dds");   //initialise the flat terrain.
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}




	// Create the timer object.
	m_Timer = new TimerClass;
	if(!m_Timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}

	// Create the position object.
	m_Position = new PositionClass;
	if(!m_Position)
	{
		return false;
	}

	// Set the initial position of the viewer to the same as the initial camera position.
	m_Position->SetPosition(cameraX, cameraY, cameraZ);
	m_Position->SetRotation(cameraRoll, 0.0f,0.0f);
	// Create the fps object.
	m_Fps = new FpsClass;
	if(!m_Fps)
	{
		return false;
	}

	// Initialize the fps object.
	m_Fps->Initialize();

	// Create the cpu object.
	m_Cpu = new CpuClass;
	if(!m_Cpu)
	{
		return false;
	}

	// Initialize the cpu object.
	m_Cpu->Initialize();

	

	// Create the text object.
	m_Text = new TextClass;
	if(!m_Text)
	{
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// Retrieve the video card information.
	m_Direct3D->GetVideoCardInfo(videoCard, videoMemory);

	// Set the video card information in the text object.
	result = m_Text->SetVideoCardInfo(videoCard, videoMemory, m_Direct3D->GetDeviceContext());
	if(!result)
	{
		MessageBox(hwnd, L"Could not set video card info in the text object.", L"Error", MB_OK);
		return false;
	}

	

	// Create the light object.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(0.0f, 0.0f, 0.0f, 1.0f);
	m_Light->SetDiffuseColor(0.0f, 0.0f, 0.0f, 1.0f);
	m_Light->SetDirection(0.0f,0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(200.0f);

	//Initialize point lights

	for (int i = 0; i < NUM_LIGHTS; i++) {
		m_PointLights[i] = new PointLightClass;
		//if not initialized
		if (!m_PointLights[i]) return false;

		m_PointLights[i]->SetRadius(25.0f);
		m_PointLights[i]->SetFallOffDistance(15.0f);
		m_PointLights[i]->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	}

	//setting values to point lights
	m_PointLights[0]->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_PointLights[0]->SetRadius(13.0f);
	m_PointLights[0]->SetFallOffDistance(5.0f);

	m_PointLights[1]->SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f);

	m_PointLights[2]->SetDiffuseColor(0.0f, 0.0f, 10.0f, 1.0f);

	m_PointLights[3]->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	
	
	

	// Create the quad tree object.
	m_QuadTree = new QuadTreeClass;
	if (!m_QuadTree)
	{
		return false;
	}

	// Initialize the quad tree object.
	result = m_QuadTree->Initialize(m_Terrain, m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the quad tree object.", L"Error", MB_OK);
		return false;
	}
	
	
	
	//initialize gameManager
	m_gameManager = new GameManager;
	result = m_gameManager->Initialize(m_Direct3D->GetDevice(),hwnd,m_Input,m_Light,m_PointLights, m_QuadTree ,m_Camera);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the Game Manager.", L"Error", MB_OK);
		return false;
	}


	//obtaining collectables position from the terrain:
	vector<D3DXVECTOR3> vc;
	m_Terrain->GetCollectablePoints(vc, NUM_COLLECTABLES);
	
	//setting obtained locations to game manager
	m_gameManager->SetPlayerAndOthersLocation(m_Terrain->GetPlayerStart(),vc);

	m_renderManager = new RenderManagerClass;
	if (!m_renderManager)
		return false;

	//	initializing the renderManager in the end so that all the pointers are passed properly
	result = m_renderManager->Initialize(hwnd, screenWidth, screenHeight, m_Direct3D, m_Camera, m_Terrain, m_Text, m_QuadTree, m_Light, m_PointLights, m_gameManager);
	if (!result) return false;

	return true;
}



void ApplicationClass::Shutdown()
{
	//releasing render manager first
	if (m_renderManager) {
		m_renderManager->Shutdown();
		delete m_renderManager;
		m_renderManager = 0;
	}

	//release gameManager
	if (m_gameManager) {
		m_gameManager->Shutdown();
		m_gameManager = 0;
	}
	//release the point lights
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		if (m_PointLights[i]) 
		{
			delete m_PointLights[i];
			m_PointLights[i] = 0;
		}
	}


	// Release the quad tree object.
	if (m_QuadTree)
	{
		m_QuadTree->Shutdown();
		delete m_QuadTree;
		m_QuadTree = 0;
	}

	


	// Release the light object.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}


	// Release the text object.
	if(m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	

	// Release the cpu object.
	if(m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// Release the fps object.
	if(m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	// Release the position object.
	if(m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	// Release the timer object.
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the terrain object.
	if(m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if(m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	// Release the input object.
	if(m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	return;
}


bool ApplicationClass::Frame()
{
	bool result;


	// Read the user input.
	result = m_Input->Frame();
	if(!result)
	{
		return false;
	}

	// Check if the user pressed escape and wants to exit the application.
	if(m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	// Update the system stats.
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	// Update the FPS value in the text object.
	result = m_Text->SetFps(m_Fps->GetFps(), m_Direct3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}
	
	// Update the CPU usage value in the text object.
	result = m_Text->SetCpu(m_Cpu->GetCpuPercentage(), m_Direct3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	// Do the frame input processing.
	result = HandleInput(m_Timer->GetTime());
	if(!result)
	{
		return false;
	}

	//handle inputs in gameManager
	if (!m_freeCam) {
		m_gameManager->Frame(m_Timer->GetTime());
	}

	//setting text objects
	result = m_Text->SetCollectablesValue(m_gameManager->GetRemainingCount(), m_renderManager->m_radialBlur, m_renderManager->m_bloomEnabled, m_Direct3D->GetDeviceContext());
	if (!result)	return false;

	//set free cam texton or off
	result = m_Text->SetFreeCamMode(m_freeCam, m_Direct3D->GetDeviceContext());
	if (!result)	return false;


	// Render the graphics.
	result = Render();
	if(!result)
	{
		return false;
	}

	return result;
}


bool ApplicationClass::HandleInput(float frameTime)
{
	bool keyDown, result;


	// Set the frame time for calculating the updated position.
	m_Position->SetFrameTime(frameTime);
	
	//switch between Game mode and free movement mode
	keyDown = m_Input->IsLPressedOnce();
	if (keyDown) {
		m_freeCam = !m_freeCam;
		if (!m_freeCam) {
			m_Light->SetDiffuseColor(0.0f, 0.0f, 0.0f, 1.0f);
			m_Light->SetAmbientColor(0.0f, 0.0f, 0.0f, 1.0f);
		}
		else {
			m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
			m_Light->SetAmbientColor(0.6f, 0.6f, 0.6f, 1.0f);
		}

	}

	keyDown = m_Input->IsBPressedOnce();
	if (keyDown) {
		m_renderManager->m_radialBlur = !m_renderManager->m_radialBlur;
	}
	keyDown = m_Input->IsGPressedOnce();
	if (keyDown) {
		m_renderManager->m_bloomEnabled = !m_renderManager->m_bloomEnabled;
	}



	//if freecam mode is on
	if (m_freeCam) 
	{
		//Refresh Terrain
		keyDown = m_Input->IsRPressedOnce();
		if (keyDown) {
			m_Terrain->RefreshTerrain(m_Direct3D->GetDevice(), keyDown);
			m_QuadTree->ReinitializeBuffers(m_Terrain, m_Direct3D->GetDevice());
		}

		//Randomized Noise
		keyDown = m_Input->IsSpacePressed();
		if (keyDown) {
			m_Terrain->AddRandomNoise(m_Direct3D->GetDevice(), keyDown);
			m_QuadTree->ReinitializeBuffers(m_Terrain, m_Direct3D->GetDevice());
		}

		//Smoothing
		keyDown = m_Input->IsSPressedOnce();
		if (keyDown) {
			m_Terrain->SmoothTerrain(m_Direct3D->GetDevice(), keyDown);
			m_QuadTree->ReinitializeBuffers(m_Terrain, m_Direct3D->GetDevice());
		}

		//perlin noise
		keyDown = m_Input->IsXPressedOnce();
		if (keyDown) {
			m_Terrain->PassThroughPerlinNoise(m_Direct3D->GetDevice(), keyDown);
			m_QuadTree->ReinitializeBuffers(m_Terrain, m_Direct3D->GetDevice());
		}

		//Faulting
		keyDown = m_Input->IsFPressedOnce();
		if (keyDown) {
			m_Terrain->Faulting(m_Direct3D->GetDevice(), keyDown);
			m_QuadTree->ReinitializeBuffers(m_Terrain, m_Direct3D->GetDevice());
		}

		keyDown = m_Input->IsVPressedOnce();
		if (keyDown) {
			m_Terrain->VoronoiRegions(m_Direct3D->GetDevice(), keyDown);
			m_QuadTree->ReinitializeBuffers(m_Terrain, m_Direct3D->GetDevice());

			//obtaining collectables position from the terrain:
			vector<D3DXVECTOR3> vc;
			m_Terrain->GetCollectablePoints(vc, NUM_COLLECTABLES);
			//setting obtained locations to game manager
			m_gameManager->SetPlayerAndOthersLocation(m_Terrain->GetPlayerStart(), vc);
		}
		//camera Movement

		keyDown = m_Input->IsLeftPressed();
		m_Position->TurnLeft(keyDown);

		keyDown = m_Input->IsRightPressed();
		m_Position->TurnRight(keyDown);

		keyDown = m_Input->IsUpPressed();
		m_Position->MoveForward(keyDown);

		keyDown = m_Input->IsDownPressed();
		m_Position->MoveBackward(keyDown);

		keyDown = m_Input->IsAPressed();
		m_Position->MoveUpward(keyDown);

		keyDown = m_Input->IsZPressed();
		m_Position->MoveDownward(keyDown);

		keyDown = m_Input->IsPgUpPressed();
		m_Position->LookUpward(keyDown);

		keyDown = m_Input->IsPgDownPressed();
		m_Position->LookDownward(keyDown);

		result = SetCameraMovement();
		if (!result)	return false;
	}

	return true;
}


bool ApplicationClass::SetCameraMovement() 
{
	bool result, foundHeight;
	float posX, posY, posZ, rotX, rotY, rotZ; 
	float height;

	// Get the view point position/rotation. Input for this is setup in HandleInput
	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);
	float newPosY = posY;

	// Get the height of the triangle that is directly underneath the given camera position.
	bool canWalk = false;
	foundHeight = m_QuadTree->GetHeightAtPosition(posX, posZ, height,canWalk);
	if (foundHeight)
	{
		// If there was a triangle under the camera then position the camera just above it by two units.
		if (posY < 3.0f) {
			newPosY = height + 2.0f;
			if (!canWalk) {
				//colission
				posX = m_Camera->m_prevX;
				posZ = m_Camera->m_prevZ;
				m_Position->SetPosition(posX, posY, posZ);
				newPosY = m_Camera->m_prevY;
			}
		}
	}

	// Set the position of the camera.
	m_Camera->SetPosition(posX, newPosY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);


	return true;
}
bool ApplicationClass::Render() {
	m_renderManager->Render();
	return true;
}