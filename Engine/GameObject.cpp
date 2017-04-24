#include "GameObject.h"
#include "Utils.h"
#include <math.h>
GameObject::GameObject()
{
	m_x = 0;
	m_y = 0;
	m_z = 0;
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
	modelName = "../Engine/data/cube.txt";
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
