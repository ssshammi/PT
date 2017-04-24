#include "PlayerClass.h"
#include "Utils.h"
PlayerClass::PlayerClass()
{
	m_camera = 0;
	//m_scale.x = 2.0f;

	m_scale.x = 0.5f;
	m_scale.y = 0.5f;
	m_scale.z = 0.5f;
}

PlayerClass::PlayerClass(const PlayerClass &)
{
}

PlayerClass::~PlayerClass()
{
}

bool PlayerClass::Initialize(ID3D11Device * device, HWND hwnd, InputClass * input, QuadTreeClass* quadTree, CameraClass * camera)
{
	bool result;
	result = GameObject::Initialize(device, hwnd, input, quadTree);
	if (!result) return false;

	m_camera = camera;
	if (!m_camera)	return false;

	m_position->SetAcceleration(0.001f, 0.0009f, 0.023f);

	return true;
}

void PlayerClass::Shutdown()
{
	GameObject::Shutdown();
	if (m_camera) {
		m_camera = 0;
	}
}

//Player only functionalities
void PlayerClass::HandleInput(float frameTime)
{
	//m_position->SetFrameTime(frameTime);
	const float pi = 3.14f*2.0f;
	bool keyDown;
	keyDown = m_input->IsUpPressed();
	m_position->MoveForward(keyDown);
	if (keyDown)	m_newYaw = pi*0.0f;

	keyDown = m_input->IsDownPressed();
	m_position->MoveBackward(keyDown);
	if (keyDown)	m_newYaw = pi*0.5f;

	keyDown = m_input->IsRightPressed();
	m_position->MoveRight(keyDown);
	if (keyDown)	m_newYaw = pi*0.25f;

	keyDown = m_input->IsLeftPressed();
	m_position->MoveLeft(keyDown);
	if (keyDown)	m_newYaw = pi* 0.75f;


	m_yaw = Utils::LerpRadians(m_yaw, m_newYaw, 0.25f);

	SetHeight();
	if (m_attachedLight)		SetLightPosition();
	SetCameraPosition();
	return;
}

void PlayerClass::GetModelAndTexture(char *& modelName, WCHAR *& textureName)
{
	modelName = "../Engine/data/cube.txt";
	textureName = L"../Engine/data/player.dds";
}

void PlayerClass::SetHeight()
{
	float x, y, z;
	float height;
	bool walkable = false;
	m_position->GetPosition(x, y, z);
	float newPosx = x, newPosy = y, newPosz = z;
	bool found = m_quadTree->GetHeightAtPosition(x, z, height, walkable);
	if (found) {
		newPosy = height + 1.0f;
		if (!walkable) {
			newPosx = m_xprev;
			newPosz = m_zprev;
			newPosy = m_yprev;
		}
	}


	SetPosition(newPosx, newPosy, newPosz);
	return;
}

void PlayerClass::SetCameraPosition()
{
	D3DXVECTOR3 camPos;
	float plx, ply, plz;

	camPos = m_camera->GetPosition();

	m_position->GetPosition(plx, ply, plz);
	D3DXVECTOR3 newCamPos;
	float screenHeight = (float)m_input->m_screenHeight, screenWidth = (float)m_input->m_screenWidth;
	//new z position
	plz = plz - (16.0f*screenHeight*800.0f / (screenWidth*600.0f));
	D3DXVec3Lerp(&newCamPos, &camPos, &D3DXVECTOR3(plx, ply + (36.0f/**screenHeight/600.0f*/), plz), 0.15f);
	//smoothen camera
	m_camera->SetPosition(newCamPos.x, newCamPos.y, newCamPos.z);	//make z substract by ScreenHeight/2
	m_camera->SetRotation(70.0f, 0.0f, 0.0f);

}

void PlayerClass::SetLightPosition()
{

	float forwardDist = 1.1f;
	// Convert degrees to radians.
	float radians = m_yaw;

	//Set light[0] position to player position
	float newx = m_x + sinf(radians) * forwardDist;
	float newz = m_z + cosf(radians) * forwardDist;
	m_attachedLight->SetPosition(newx, m_y + forwardDist, newz);


	//setting light intensity
	float randa = Utils::RandomFloat(-0.1f, 0.1f);

	m_attachedLight->SetDiffuseColor(m_initIntensity.x + randa, m_initIntensity.y + randa, m_initIntensity.z + randa, m_initIntensity.w);
}
