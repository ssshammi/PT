#pragma once
#ifndef _POINTLIGHTHEADERDATA_H_
#define _POINTLIGHTHEADERDATA_H_

//GLOBALS
const int NUM_LIGHTS = 10;

//Point light buffers
struct PointLightColorBufferType
{
	D3DXVECTOR4 diffuseColor[NUM_LIGHTS];
	D3DXVECTOR4 PointLightRadius[NUM_LIGHTS];
};

struct PointLightPositionBufferType
{
	D3DXVECTOR4 lightPosition[NUM_LIGHTS];
};


#endif