

////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pointlightclass.h"


PointLightClass::PointLightClass()
{
}


PointLightClass::PointLightClass(const PointLightClass& other)
{
}


PointLightClass::~PointLightClass()
{
}


void PointLightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}


void PointLightClass::SetPosition(float x, float y, float z)
{
	m_position = D3DXVECTOR4(x, y, z, 1.0f);
	return;
}

void PointLightClass::SetRadius(float newRadius)
{
	m_radius = newRadius;
}

void PointLightClass::SetFallOffDistance(float NewValue)
{
	m_fallOffDistance = NewValue;
}


D3DXVECTOR4 PointLightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


D3DXVECTOR4 PointLightClass::GetPosition()
{
	return m_position;
}

float PointLightClass::GetRadius()
{
	return m_radius;
}

float PointLightClass::GetFallOffDistance()
{
	return m_fallOffDistance;
}
