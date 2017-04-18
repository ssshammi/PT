#pragma once

#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_

#include "GameObject.h"
#include "lightclass.h"
#include "pointlightclass.h"
#include "cameraclass.h"


const int NUM_COLLECTABLES = 10;
class GameManager {
public:
	GameManager();
	GameManager(const GameManager&);
	~GameManager();

	bool Initialize(ID3D11Device* device, HWND hwnd, InputClass *input, LightClass *DirectionalLight, PointLightClass* PointLights[], QuadTreeClass* quadTree,CameraClass *camera);
	void Frame(float frametime);
	bool Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
		FrustumClass *frustum, D3DXVECTOR4 pointLightColors[], D3DXVECTOR4 pointLightPositions[],	float pointLightRadius[], float pointFallOutDist[], int &nFrustum);
	void Shutdown();
	void SetPlayerAndOthersLocation(D3DXVECTOR3 playerPos, vector<D3DXVECTOR3> vc);
	int GetRemainingCount();

private:
	void HandleInput( float frameTime);
	CameraClass *m_camera;
	InputClass *m_input;
	LightClass *m_directionalLight;
	PointLightClass **m_pointLights;
	PlayerClass *m_playerObject;
	CollectablesClass * m_Collectables[NUM_COLLECTABLES];
	int m_remaining;
};




#endif // !_GAMEMANAGER_H_
