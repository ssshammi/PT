#include "GameObject.h"
#include "Utils.h"
#include <math.h>
GameObject::GameObject()
{
	m_input = 0;
	m_position = 0;
	m_lightshader = 0;
	m_model = 0;
	m_xprev = 0.0f;
	m_yprev = 0.0f;
	m_zprev = 0.0f;
	enabled = true;
	m_attachedLight = 0;
	m_roll = 0.0f;
	m_pitch = 0.0f;
	m_yaw = 0.0f;

	m_scale.x = 1.0f;
	m_scale.y = 1.0f;
	m_scale.z = 1.0f;
}

GameObject::GameObject(const GameObject &)
{
}

GameObject::~GameObject()
{
}

bool GameObject::Initialize(ID3D11Device* device, HWND hwnd, InputClass *input, QuadTreeClass* quadTree)
{
	//get name of model from specific type of class
	char* modelName;
	WCHAR* TextureName;

	GetModelAndTexture(modelName,TextureName);

	bool result;


	m_input = input;
	if (!m_input) return false;

	m_quadTree = quadTree;
	if (!m_quadTree) return false;

	m_model = new ModelClass;
	if (!m_model) return false;

	result = m_model->Initialize(device, modelName,TextureName);
	if (!result) return false;


	m_lightshader = new LightShaderClass;
	if (!m_lightshader) return false;

	result = m_lightshader->Initialize(device, hwnd);
	if (!result) return false;

	m_position = new PositionClass;
	if (!m_position) return false;

	return true;
}

void GameObject::Frame(float frameTime)
{
	HandleInput(frameTime);
}

bool GameObject::Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
						D3DXMATRIX projectionMatrix, FrustumClass *frustum, D3DXVECTOR3 LightDirection, D3DXVECTOR4 ambientColor,
						D3DXVECTOR4 diffusedColor, D3DXVECTOR3 CameraPos, D3DXVECTOR4 SpecularColor, 
						float specularPower, D3DXVECTOR4 pointLightColors[], D3DXVECTOR4 pointLightPositions[], 
						float pointLightRadius[], float pointFallOutDist[], int& nFrustum)
{
	bool result, renderModel;
	float pos_x, pos_y, pos_z;
	float radius = 1.0f;
	//get current position of object
	GetCurrentPosition(pos_x,pos_y,pos_z);

	renderModel = frustum->CheckCube(pos_x, pos_y, pos_z, radius);
	if (renderModel) 
	{
		D3DXMATRIX S, R, T, M;

		D3DXMatrixScaling(&S, m_scale.x, m_scale.y, m_scale.z);
		D3DXMatrixTranslation(&T, pos_x, pos_y, pos_z);
		D3DXMatrixRotationYawPitchRoll(&R, m_yaw, m_pitch, m_roll);
		M = worldMatrix;
		D3DXMatrixMultiply(&M, &M, &S);
		D3DXMatrixMultiply(&M, &M, &R);
		D3DXMatrixMultiply(&M, &M, &T);

		//moving the object to the desired location using temporoy world position matrix
		/*D3DXMatrixRotationYawPitchRoll(&tempWorldMatrix, m_yaw,m_pitch,m_roll);
		D3DXMatrixTranslation(&tempWorldMatrix, pos_x, pos_y, pos_z);
		*/m_model->Render(deviceContext);

		result = RenderWithShader(deviceContext,M,viewMatrix,projectionMatrix);
		if (!result) 
		{
			// Render the model using the light shader.
			result = m_lightshader->Render(deviceContext, m_model->GetIndexCount(), M, viewMatrix, projectionMatrix,
				m_model->GetTexture(), LightDirection, ambientColor, diffusedColor, CameraPos,
				SpecularColor, specularPower, pointLightColors, pointLightPositions, pointLightRadius, pointFallOutDist);

			if (!result) {
				return false;
			}
		}

		nFrustum++;
	}

	return true;
}

void GameObject::Shutdown()
{
	if (m_input)
		m_input = 0;

	if (m_lightshader) {
		m_lightshader->Shutdown();
		m_lightshader = 0;
	}

	if (m_model) {
		m_model->Shutdown();
		m_model = 0;
	}

	if (m_position) {
		delete m_position;
		m_position = 0;
	}

	return;
}

void GameObject::SetPosition(float x, float y, float z)
{
	//setting prev position
	m_xprev = m_x;
	m_yprev = m_y;
	m_zprev = m_z;

	//setting new pos;
	m_x = x;
	m_y = y;
	m_z = z;
	m_position->SetPosition(x,y,z);
	return;
}

void GameObject::SetPosition(D3DXVECTOR3 v, bool init)
{
	if (init)
	{
		m_initPos = v;
	}
	SetPosition(v.x, v.y, v.z);
	return;
}

void GameObject::SetScale(float f, bool init)
{
	D3DXVECTOR3 v = D3DXVECTOR3(f, f, f);
	SetScale(v,init);
	
}

void GameObject::SetScale(D3DXVECTOR3 v, bool init)
{
	if (init) {
		m_initScale = v;
	}
	m_scale = v;
}

void GameObject::SetRotation(D3DXVECTOR3 v, bool init)
{
	if (init) {
		m_initRotation = v;
	}
	m_yaw = v.x;
	m_pitch = v.y;
	m_roll = v.z;
}

void GameObject::SetRotation(float yaw, float pitch, float roll, bool init)
{
	D3DXVECTOR3 v = D3DXVECTOR3(yaw,pitch,roll);
	SetRotation(v, init);
}

D3DXVECTOR3 GameObject::GetPosition()
{
	return D3DXVECTOR3(m_x,m_y,m_z);
}

void GameObject::AttachLight(PointLightClass * light)
{
	m_attachedLight = light;
	m_initIntensity = m_attachedLight->GetDiffuseColor();
}

bool GameObject::RenderWithShader(ID3D11DeviceContext* deviceContext, D3DMATRIX world, D3DMATRIX view, D3DMATRIX projection)
{
	return false;
}

void GameObject::HandleInput(float frameTime)
{
	//m_position->SetFrameTime(frameTime);

	bool keyDown;
	keyDown = m_input->IsAPressed();
	m_position->MoveForward(keyDown);


	keyDown = m_input->IsZPressed();
	m_position->MoveBackward(keyDown);
}

void GameObject::GetModelAndTexture(char *& modelName, WCHAR *& textureName)
{
	modelName = "../Engine/data/cube.obj";
	textureName = L"../Engine/data/white.dds";
	
}

void GameObject::GetCurrentPosition(float &x, float &y, float &z)
{

	//setting the xprev and y prev
	//m_xprev = m_x; m_yprev = m_y, m_zprev = m_z;
	//m_position->GetPosition(x,y,z);

	x = m_x;
	y = m_y;
	z = m_z;
	return;
}


PlayerClass::PlayerClass()
{
	m_camera = 0;
	//m_scale.x = 2.0f;
}

PlayerClass::PlayerClass(const PlayerClass &)
{
}

PlayerClass::~PlayerClass()
{
}

bool PlayerClass::Initialize(ID3D11Device * device, HWND hwnd, InputClass * input,QuadTreeClass* quadTree, CameraClass * camera)
{
	bool result;
	result = GameObject::Initialize(device,hwnd,input, quadTree);
	if (!result) return false;

	m_camera = camera;
	if (!m_camera)	return false;

	m_position->SetAcceleration(0.001f,0.0009f,0.023f);

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
	if (keyDown)	m_newYaw =pi* 0.75f;


	m_yaw = Utils::LerpRadians(m_yaw,m_newYaw,0.25f);

	SetHeight();
	if(m_attachedLight)		SetLightPosition();
	SetCameraPosition();
	return;
}

void PlayerClass::GetModelAndTexture(char *& modelName, WCHAR *& textureName)
{
	modelName = "../Engine/data/cube.obj";
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

	m_position->GetPosition(plx,ply,plz);
	D3DXVECTOR3 newCamPos;
	float screenHeight =(float) m_input->m_screenHeight, screenWidth = (float)m_input->m_screenWidth;
	//new z position
	plz = plz - (16.0f*screenHeight*800.0f / (screenWidth*600.0f));
	D3DXVec3Lerp(&newCamPos, &camPos, &D3DXVECTOR3(plx, ply + (36.0f/**screenHeight/600.0f*/),plz ),0.15f);
	//smoothen camera
	m_camera->SetPosition(newCamPos.x,newCamPos.y,newCamPos.z);	//make z substract by ScreenHeight/2
	m_camera->SetRotation(70.0f,0.0f,0.0f);

}

void PlayerClass::SetLightPosition()
{

	float forwardDist = 1.1f;
	// Convert degrees to radians.
	float radians = m_yaw;

	//Set light[0] position to player position
	float newx = m_x + sinf(radians) * forwardDist;
	float newz = m_z + cosf(radians) * forwardDist;
	m_attachedLight->SetPosition(newx, m_y+forwardDist, newz);


	//setting light intensity
	float randa = Utils::RandomFloat(-0.1f, 0.1f);

	m_attachedLight->SetDiffuseColor(m_initIntensity.x + randa, m_initIntensity.y + randa, m_initIntensity.z + randa, m_initIntensity.w);
}

CollectablesClass::CollectablesClass()
{
	m_player = 0;
	m_radius = 1.5f;
	m_timer = 0.0f;
	m_scale *= 0.8f;
	m_colorShader = 0;
}

CollectablesClass::CollectablesClass(const CollectablesClass &)
{
}

CollectablesClass::~CollectablesClass()
{
}

bool CollectablesClass::Initialize(ID3D11Device * device, HWND hwnd, InputClass * input, QuadTreeClass * quadTree, PlayerClass * player)
{
	GameObject::Initialize(device,hwnd,input,quadTree);
	m_player = player;
	if (!m_player)	return false;

	m_colorShader = new ColorShaderClass;
	m_colorShader->Initialize(device, hwnd);
	if (!m_colorShader)	return false;

	return true;
}

void CollectablesClass::HandleInput(float frametTime)
{
	m_timer += PositionClass::m_frameTime;
	D3DXVECTOR3 v = m_player->GetPosition() - GetPosition();
	float dist = D3DXVec3Length(&v);
	//check for collision with player
	if (dist<=(m_radius)) {
		enabled = false;
	}

	if (enabled) {
		//rotate little
		m_yaw += 0.05f;
		//moving up and down slightly
		const float pi = 3.14F;
		const float frequency = 10.0f; // Frequency in Hz
		float yval = 0.5f * (1.0f + sin(2.0f * pi * m_timer/(frequency*180.0f)));
		SetPosition(m_x,m_initPos.y+yval,m_z);
	}
	else {
		if (m_attachedLight) {
			D3DXVECTOR4 l = m_attachedLight->GetDiffuseColor();
			if (l.x+l.y+l.z > 0.0001f)//check if almost zero
			{
				D3DXVECTOR4 v;
				D3DXVec4Lerp(&v,&l,&D3DXVECTOR4(0.0f,0.0f,0.0f,1.0f),0.05f);
				m_attachedLight->SetDiffuseColor(v.x, v.y, v.z, v.w);
			}
		}
	}
}



void CollectablesClass::ResetCollectable() {
	enabled = true;
	if (m_attachedLight) {
		m_attachedLight->SetDiffuseColor(m_initIntensity.x, m_initIntensity.y, m_initIntensity.z, m_initIntensity.w);
	}
}

bool CollectablesClass::RenderWithShader(ID3D11DeviceContext* deviceContext, D3DMATRIX world, D3DMATRIX view, D3DMATRIX projection) {
	m_colorShader->Render(deviceContext, m_model->GetIndexCount(),world,view,projection);
	return true;
}

void CollectablesClass::Shutdown() {
	GameObject::Shutdown();
	if (m_player)
		m_player = 0;

	if (m_colorShader) {
		m_colorShader->Shutdown();
		delete m_colorShader;
		m_colorShader = 0;
	}
}