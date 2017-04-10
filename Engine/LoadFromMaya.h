#pragma once
#ifndef _LOADFROMMAYA_
#define _LOADFROMMAYA_

////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////


//////////////
// INCLUDES //
//////////////
#include <iostream>
#include <fstream>
using namespace std;

class LoadFromMaya {
public:
	LoadFromMaya();
	~LoadFromMaya();
	LoadFromMaya(const LoadFromMaya&);

private:
	typedef struct
	{
		float x, y, z;
	}VertexType;

	typedef struct
	{
		int vIndex;
		int tIndex;
		int nIndex;
	}FaceType;

	typedef struct
	{
		float v1, v2, v3;
		float t1, t2;
		float n1, n2, n3;
	}FinalFaces;

	int m_numberOfFaces;
	/////////////////////////
	// FUNCTION PROTOTYPES //
	/////////////////////////
	bool ReadFileCounts(char*, int&, int&, int&, int&);
	bool LoadDataStructures(char*, int, int, int, int, char&);
public:
	bool LoadFromMayaNow(char*, char&, int&);
#endif
};