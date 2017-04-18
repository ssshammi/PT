#pragma once

////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _POINTLIGHTCLASS_H_
#define _POINTLIGHTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: PointLightClass
////////////////////////////////////////////////////////////////////////////////
class PointLightClass
{
public:
	PointLightClass();
	PointLightClass(const PointLightClass&);
	~PointLightClass();

	void SetDiffuseColor(float, float, float, float);
	void SetPosition(float, float, float);
	void SetPosition(D3DXVECTOR3);
	void SetRadius(float newRadius);
	void SetFallOffDistance(float value);

	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR4 GetPosition();
	float GetRadius();
	float GetFallOffDistance();
private:
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR4 m_position;
	float m_radius = 25.0f;
	float m_fallOffDistance = 10.0f;
};

#endif