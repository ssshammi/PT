
#ifndef _FAULTINGCLASS_H_
#define _FAULTINGCLASS_H_

#include "Voronoi.h"
class FaultingClass 
{
public:
	FaultingClass();
	FaultingClass(const FaultingClass&);
	~FaultingClass();

	void Faulting(int m_terrainWidth, int m_terrainHeight, HeightMapType* &m_heightMap);
};

#endif