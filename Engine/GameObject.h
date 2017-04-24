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
#include "colorshaderclass.h"

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

	void SetScale(float f, bool init = false);
	void SetScale(D3DXVECTOR3 v, bool init = false);

	void SetRotation(D3DXVECTOR3 v, bool init = false);
	void SetRotation(float yaw, float pitch, float roll, bool init = false);


	D3DXVECTOR3 GetPosition();
	void AttachLight(PointLightClass* l);

public:
	bool enabled;

protected:
	virtual bool RenderWithShader(ID3D11DeviceContext* deviceContext, D3DMATRIX world, D3DMATRIX view, D3DMATRIX projection);
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
	D3DXVECTOR3 m_scale;
	float m_xprev, m_yprev, m_zprev;
	float m_roll, m_pitch, m_yaw;
	D3DXVECTOR3 m_initPos, m_initScale,m_initRotation;	//initial rotation and position and scale values
	D3DXVECTOR4 m_initIntensity;

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
	void SetLightPosition();

protected:
	CameraClass *m_camera;
	float m_newYaw;		//Storing the desired yaw for the player's looking direction

};


class CollectablesClass : public GameObject {
public:
	CollectablesClass();
	CollectablesClass(const CollectablesClass&);
	~CollectablesClass();
	virtual bool Initialize(ID3D11Device* device, HWND hwnd, InputClass *input, QuadTreeClass *quadTree, PlayerClass *player);
	void ResetCollectable();
	virtual void Shutdown();

protected:
	virtual bool RenderWithShader(ID3D11DeviceContext* deviceContext, D3DMATRIX world, D3DMATRIX view, D3DMATRIX projection);
	virtual void HandleInput(float frametTime);


private:
	ColorShaderClass* m_colorShader;
	PlayerClass* m_player;
	float m_radius;	//radius for the collision of the collectable
	float m_timer;
	
};

#endif // !_GAMEOBJECT_H_

