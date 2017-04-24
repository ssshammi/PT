#include "faultingTerrain.h"
#include "Utils.h"

FaultingClass::FaultingClass()
{

}

FaultingClass::FaultingClass(const FaultingClass &)
{
}

FaultingClass::~FaultingClass()
{
}

void FaultingClass::Faulting(int m_terrainWidth, int m_terrainHeight, HeightMapType* &m_heightMap)
{
	int x1, y1, x2, y2;
	float m, b;
	x1 = (int)m_terrainWidth*0.1f + (int)(rand() % (int)(m_terrainWidth*0.8f));
	y1 = (rand() % 2 == 0) ? m_terrainHeight - 1 : 0;
	int random1 = 0;
	while (random1 == 0 || x2 == x1) {
		random1 = (int)Utils::RandomFloat(-5.0f, 5.0f);
		x2 = x1 + random1;		//random between -15 and 15
	}
	int random2 = 0;
	while (random2 == 0)
		random2 = (int)Utils::RandomFloat(-5.0f, 5.0f);//(25 - (rand() % 50));
	y2 = y1 + random2;		//random between -15 and 15

	m = ((float)(y2 - y1)) / ((float)(x2 - x1));
	b = (float)y1 - (((float)x1)*m);


	float H1 = Utils::RandomFloat(-5.0f, 5.0f);

	float H2 = H1*0.5f;

	int index;
	bool eq;

	int i, j;
	//Faulting
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainWidth * j) + i;

			eq = (float)j >((float)i*m) + b;
			if (eq)
				m_heightMap[index].y += H1;

			m_heightMap[index].y -= H1;

		}
	}

	return;
}
