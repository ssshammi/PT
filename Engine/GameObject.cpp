#include "GameObject.h"

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
	D3DXMATRIX tempWorldMatrix = worldMatrix;
	float pos_x, pos_y, pos_z;
	float radius = 1.0f;
	//get current position of object
	GetCurrentPosition(pos_x,pos_y,pos_z);

	renderModel = frustum->CheckCube(pos_x, pos_y, pos_z, radius);
	if (renderModel) 
	{

		//moving the object to the desired location using temporoy world position matrix
		D3DXMatrixTranslation(&tempWorldMatrix, pos_x, pos_y, pos_z);
		m_model->Render(deviceContext);

		// Render the model using the light shader.
		result = m_lightshader->Render(deviceContext, m_model->GetIndexCount(), tempWorldMatrix, viewMatrix, projectionMatrix,
			m_model->GetTexture(), LightDirection, ambientColor, diffusedColor, CameraPos,
			SpecularColor, specularPower, pointLightColors, pointLightPositions, pointLightRadius, pointFallOutDist);

		if (!result) {
			return false;
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

D3DXVECTOR3 GameObject::GetPosition()
{
	return D3DXVECTOR3(m_x,m_y,m_z);
}

void GameObject::AttachLight(PointLightClass * l)
{
	m_attachedLight = l;
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
	textureName = L"../Engine/data/bricks.dds";
	
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

	bool keyDown;
	keyDown = m_input->IsUpPressed();
	m_position->MoveForward(keyDown);


	keyDown = m_input->IsDownPressed();
	m_position->MoveBackward(keyDown);

	keyDown = m_input->IsRightPressed();
	m_position->MoveRight(keyDown);

	keyDown = m_input->IsLeftPressed();
	m_position->MoveLeft(keyDown);




	SetHeight();

	SetCameraPosition();
	return;
}

void PlayerClass::GetModelAndTexture(char *& modelName, WCHAR *& textureName)
{
	modelName = "../Engine/data/cube.obj";
	textureName = L"../Engine/data/rock.dds";
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

CollectablesClass::CollectablesClass()
{
	m_player = 0;
	m_radius = 1.0f;
	m_timer = 0.0f;
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

	return true;
}

void CollectablesClass::HandleInput(float frametTime)
{
	m_timer += PositionClass::m_frameTime;
	D3DXVECTOR3 v = m_player->GetPosition() - GetPosition();
	float dist = D3DXVec3Length(&v);

	if (dist<=m_radius) {
		enabled = false;
		if (m_attachedLight)
			m_attachedLight->SetDiffuseColor(0.0f, 0.0f, 0.0f, 0.0f);
	}

	if (enabled) {
		const float pi = 3.14F;
		const float frequency = 10.0f; // Frequency in Hz
		float yval = 0.5f * (1.0f + sin(2.0f * pi * m_timer/(frequency*180.0f)));
		SetPosition(m_x,m_initPos.y+yval,m_z);
	}
}
