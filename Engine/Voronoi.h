#pragma once
#ifndef VORNOI
#define VORNOI

#include <vector>

#include "DelaunayTriangles/vector2.h"
#include "DelaunayTriangles/triangle.h"
#include "DelaunayTriangles/delaunay.h"

using namespace std;

struct VoronoiPoint {
	float x, y, z;
	int index;
	float height;
	int RegionIndex;
};

struct VoronoiData {
	VoronoiPoint* VorPoint;
	float dist;
};


struct VoronoiRegion {
	vector<int> VRegionIndices;
	float maxDist = 0.0f;
	VoronoiPoint* vPoint;
};

struct HeightMapType
{
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
	VoronoiData *VorData;
	float walkable = 0.0f;
};

class Vornoi {
public:
	Vornoi();
	Vornoi(const Vornoi&);
	~Vornoi();

	void GenerateVoronoiDungeon(HeightMapType *hmap,int terrainWidth,int terrainHeight, vector<VoronoiRegion*> &rooms, vector<vector<HeightMapType*>> &corridors);
	void ReleaseVornoi();

private:
	void VoronoiRegions(int numVornoiPoints, int numRoomsInDungeon);
	void AddVoronoiPointAt(int index, int k);
	void DelanuayTriangles();
	bool isCircular(vector<Edge*>& edges);
	bool isCircular(int v, bool visited[], vector<int, allocator<int>> **adj, int parent);	//This one is used for recursion
	void makeCorridors(const vector<Edge*> &tree);

	float GetWalkableValue(float i, float j);
	float RandomFloat(float a, float b);

private:
	vector<VoronoiRegion*> *m_VRegions;
	vector<VoronoiPoint*> *m_VPoints;
	vector<VoronoiRegion*> m_rooms;
	vector<vector<HeightMapType*>> m_corridors;

	HeightMapType* m_heightMap;
	int m_terrainWidth;
	int m_terrainHeight;


};


#endif