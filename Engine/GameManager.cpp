#include "GameManager.h"

GameManager::GameManager()
{
	m_input = 0;
	m_camera = 0;
	m_directionalLight = 0;


	m_playerObject = 0;

	for(int i = 0;i<NUM_COLLECTABLES;i++)
		m_Collectables[i] = 0;
}

GameManager::GameManager(const GameManager &)
{
}

GameManager::~GameManager()
{
}

bool GameManager::Initialize(ID3D11Device * device, HWND hwnd, InputClass *input, LightClass *DirectionalLight, PointLightClass* PointLights[], QuadTreeClass* quadTree, CameraClass *camera)
{
	bool result;
	m_input = input;
	if (!m_input) return false;

	m_camera = camera;
	if (!m_camera)	return false;

	m_directionalLight = DirectionalLight;
	if (!m_directionalLight) return false;
	m_pointLights = PointLights;
	if (!m_pointLights) return false;

	m_playerObject = new PlayerClass;
	if (!m_playerObject) return false;
	
	result = m_playerObject->Initialize(device, hwnd, m_input,quadTree,m_camera);
	if (!result)	return false;

	//m_playerObject->SetPosition(playerStart.x, playerStart.y, playerStart.z);




	for (int i = 0; i < NUM_COLLECTABLES; i++) {
		m_Collectables[i] = new CollectablesClass;
		if (!m_Collectables[i]) return false;
		
		result = m_Collectables[i]->Initialize(device, hwnd, m_input, quadTree, m_playerObject);
		if (!result) return false;

	}


	//initial intensity of player's point light
	m_initIntensity = m_pointLights[0]->GetDiffuseColor();

	return true;
}

void GameManager::Frame(float frameTime)
{
	HandleInput(frameTime);
	//calling Player's Frame
	m_playerObject->Frame(frameTime);

	//Calling Collectable's Frame
	for(int i = 0;i<NUM_COLLECTABLES;i++)
		m_Collectables[i]->Frame(frameTime);



}

bool GameManager::Render(ID3D11DeviceContext * deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, FrustumClass *frustum,
	D3DXVECTOR4 pointLightColors[], D3DXVECTOR4 pointLightPositions[], float pointLightRadius[], float pointFallOutDist[], int &nFrustum)
{
	bool result;

	result = m_playerObject->Render(deviceContext,worldMatrix, viewMatrix, projectionMatrix, frustum, m_directionalLight->GetDirection(),m_directionalLight->GetAmbientColor(),
		m_directionalLight->GetDiffuseColor(),m_camera->GetPosition() ,m_directionalLight->GetSpecularColor(),m_directionalLight->GetSpecularPower(),
		pointLightColors ,pointLightPositions,pointLightRadius,pointFallOutDist,nFrustum);

	if (!result) return false;

	//Collectables render

	for (int i = 0; i < NUM_COLLECTABLES; i++) {
		if(m_Collectables[i]->enabled)
		result = m_Collectables[i]->Render(deviceContext, worldMatrix, viewMatrix, projectionMatrix, frustum, m_directionalLight->GetDirection(), m_directionalLight->GetAmbientColor(),
			m_directionalLight->GetDiffuseColor(), m_camera->GetPosition(), m_directionalLight->GetSpecularColor(), m_directionalLight->GetSpecularPower(),
			pointLightColors, pointLightPositions, pointLightRadius, pointFallOutDist, nFrustum);

		if (!result) return false;
	}
	

	return true;

}

void GameManager::Shutdown()
{
	if (m_Collectables) {
		for (int i = 0; i < NUM_COLLECTABLES; i++) {
			if (m_Collectables[i]) 
			{
				m_Collectables[i]->Shutdown();
				m_Collectables[i] = 0;
			}
		}
		//delete[] m_Collectables;
	}

	if (m_directionalLight)
		m_directionalLight = 0;

	if (m_input)
		m_input = 0;

	if (m_camera)
		m_camera = 0;

	if (m_playerObject)
	{
		m_playerObject->Shutdown();
		m_playerObject = 0;
	}
	if (m_pointLights)
		m_pointLights = 0;

	return;
}

void GameManager::HandleInput(float frameTime)
{
	//Set light[0] position to player position
	D3DXVECTOR3 playerPos = m_playerObject->GetPosition();
	m_pointLights[0]->SetPosition(playerPos.x, playerPos.y, playerPos.z - 1.5f);

	
	float randa = RandomFloat(-0.1f,0.1f);
	
	m_pointLights[0]->SetDiffuseColor(m_initIntensity.x + randa, m_initIntensity.y + randa,	m_initIntensity.z + randa, m_initIntensity.w);
}

float GameManager::RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

void GameManager::SetPlayerAndOthersLocation(D3DXVECTOR3 playerPos, vector<D3DXVECTOR3> vc) {
	m_playerObject->SetPosition(playerPos,true);

	for (int i = 0; i < NUM_COLLECTABLES; i++) {
		m_Collectables[i]->SetPosition(vc[i],true);
	}
	for (int i = 1; i < NUM_LIGHTS; i++) {
		m_pointLights[i]->SetPosition(vc[i]);
		m_Collectables[i]->AttachLight(m_pointLights[i]);
	}

}