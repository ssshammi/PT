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

	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR4 GetPosition();

private:
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR4 m_position;
};

#endif