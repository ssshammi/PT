#include "CollectablesClass.h"

CollectablesClass::CollectablesClass()
{
	m_player = 0;
	m_radius = 1.5f;
	m_timer = 0.0f;
	m_scale *= 0.8f;
	m_colorShader = 0;

	m_scale.x = 0.35f;
	m_scale.y = 0.35f;
	m_scale.z = 0.35f;
}

CollectablesClass::CollectablesClass(const CollectablesClass &)
{
}

CollectablesClass::~CollectablesClass()
{
}

bool CollectablesClass::Initialize(ID3D11Device * device, HWND hwnd, InputClass * input, QuadTreeClass * quadTree, PlayerClass * player)
{
	GameObject::Initialize(device, hwnd, input, quadTree);
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
	if (dist <= (m_radius)) {
		enabled = false;
	}

	if (enabled) {
		//rotate little
		m_yaw += 0.05f;
		//moving up and down slightly
		const float pi = 3.14F;
		const float frequency = 10.0f; // Frequency in Hz
		float yval = 0.5f * (1.0f + sin(2.0f * pi * m_timer / (frequency*180.0f)));
		SetPosition(m_x, m_initPos.y + yval, m_z);
	}
	else {
		if (m_attachedLight) {
			D3DXVECTOR4 l = m_attachedLight->GetDiffuseColor();
			if (l.x + l.y + l.z > 0.0001f)//check if almost zero
			{
				D3DXVECTOR4 v;
				D3DXVec4Lerp(&v, &l, &D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), 0.05f);
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
	m_colorShader->Render(deviceContext, m_model->GetIndexCount(), world, view, projection);
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