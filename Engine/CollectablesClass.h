#pragma once

#ifndef _COLLECTABLESCLASS_H_
#define _COLLECTABLESCLASS_H_
#include "PlayerClass.h"

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

