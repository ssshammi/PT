#include "GameObject.h"

GameObject::GameObject()
{
	m_input = 0;
	m_position = 0;
	m_lightshader = 0;
	m_model = 0;
}

GameObject::GameObject(const GameObject &)
{
}

GameObject::~GameObject()
{
}

bool GameObject::Initialize(ID3D11Device* device, HWND hwnd, InputClass *input)
{
	bool result;
	m_model = new ModelClass;
	if (!m_model) return false;

	result = m_model->Initialize(device, "../Engine/data/cube.obj", L"../Engine/data/bricks.dds");
	if (!result) return false;

	m_input = input;
	if (!m_input) return false;

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
	D3DXMATRIX temp = worldMatrix;
	float pos_x, pos_y, pos_z;
	float radius = 1.0f;
	m_position->GetPosition(pos_x,pos_y,pos_z);
	renderModel = frustum->CheckCube(pos_x, pos_y, pos_z, radius);
	if (renderModel) 
	{
		//moving the object to the desired location using temporoy world position matrix
		D3DXMatrixTranslation(&temp, pos_x, pos_y, pos_z);
		m_model->Render(deviceContext);

		// Render the model using the light shader.
		result = m_lightshader->Render(deviceContext, m_model->GetIndexCount(), temp, viewMatrix, projectionMatrix,
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
	m_position->SetPosition(x,y,z);
	return;
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


