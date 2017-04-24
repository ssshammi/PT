#pragma once

#ifndef _PLAYERCLASS_H_
#define _PLAYERCLASS_H_

#include "GameObject.h"

class PlayerClass : public GameObject {
public:
	PlayerClass();
	PlayerClass(const PlayerClass&);
	~PlayerClass();
	virtual bool Initialize(ID3D11Device* device, HWND hwnd, InputClass *input, QuadTreeClass *quadTree, CameraClass *camera);
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

#endif