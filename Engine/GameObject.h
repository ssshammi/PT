#pragma once

#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

//INCLUDES
#include "modelclass.h"
#include "inputclass.h"
#include "positionclass.h"
#include "lightshaderclass.h"
#include "quadtreeclass.h"
#include "cameraclass.h"
#include "pointlightclass.h"

class GameObject {
public:
	GameObject();
	GameObject(const GameObject&);
	~GameObject();

	virtual bool Initialize(ID3D11Device* device, HWND hwnd, InputClass *input, QuadTreeClass* quadTree);
	void Frame(float frameTime);
	bool Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
					FrustumClass *frustum, D3DXVECTOR3 LightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffusedColor, D3DXVECTOR3 CameraPos,
					D3DXVECTOR4 SpecularColor, float specularPower, D3DXVECTOR4 pointLightColors[], D3DXVECTOR4 pointLightPositions[], 
					float pointLightRadius[], float pointFallOutDist[], int &nFrustum);
	
	virtual void Shutdown();


	void SetPosition(float x, float y, float z);
	void SetPosition(D3DXVECTOR3 v, bool init = false);
	D3DXVECTOR3 GetPosition();
	void AttachLight(PointLightClass* l);

public:
	bool enabled;

protected:
	virtual void HandleInput(float frameTime);
	virtual void GetModelAndTexture(char* &modelName, WCHAR* &textureName);
	virtual void GetCurrentPosition(float &x, float &y, float &z);
	
protected:
	InputClass *m_input;
	PositionClass *m_position;
	ModelClass *m_model;
	LightShaderClass *m_lightshader;
	QuadTreeClass *m_quadTree;
	PointLightClass* m_attachedLight;

	float m_x, m_y, m_z;
	float m_xprev, m_yprev, m_zprev;
	D3DXVECTOR3 m_initPos;


};


class PlayerClass : public GameObject {
public:
	PlayerClass();
	PlayerClass(const PlayerClass&);
	~PlayerClass();
	virtual bool Initialize(ID3D11Device* device, HWND hwnd, InputClass *input, QuadTreeClass *quadTree,CameraClass *camera);
	virtual void Shutdown();


protected:
	virtual void HandleInput(float frametTime);
	virtual void GetModelAndTexture(char* &modelName, WCHAR* &textureName);
	void SetHeight();
	void SetCameraPosition();

protected:
	CameraClass *m_camera;

};


class CollectablesClass : public GameObject {
public:
	CollectablesClass();
	CollectablesClass(const CollectablesClass&);
	~CollectablesClass();
	virtual bool Initialize(ID3D11Device* device, HWND hwnd, InputClass *input, QuadTreeClass *quadTree, PlayerClass *player);

protected:
	virtual void HandleInput(float frametTime);

private:
	PlayerClass* m_player;
	float m_radius;	//radius for the collision of the collectable
	float m_timer;
};

#endif // !_GAMEOBJECT_H_

