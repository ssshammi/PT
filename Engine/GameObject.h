#pragma once

#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

//INCLUDES
#include "modelclass.h"
#include "inputclass.h"
#include "positionclass.h"
#include "lightshaderclass.h"
#include "quadtreeclass.h"

class GameObject {
public:
	GameObject();
	GameObject(const GameObject&);
	~GameObject();

	bool Initialize(ID3D11Device* device, HWND hwnd, InputClass *input);
	void Frame(float frameTime);
	bool Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
					FrustumClass *frustum, D3DXVECTOR3 LightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffusedColor, D3DXVECTOR3 CameraPos,
					D3DXVECTOR4 SpecularColor, float specularPower, D3DXVECTOR4 pointLightColors[], D3DXVECTOR4 pointLightPositions[], 
					float pointLightRadius[], float pointFallOutDist[], int &nFrustum);

	void Shutdown();


	void SetPosition(float x, float y, float z);
private:
	void HandleInput(float frameTime);

private:
	InputClass *m_input;
	PositionClass *m_position;
	ModelClass *m_model;
	LightShaderClass *m_lightshader;
};

#endif // !_GAMEOBJECT_H_

