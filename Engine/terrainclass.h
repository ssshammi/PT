////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <stdio.h>
#include <vector>

#include "textureclass.h"

#include "DelaunayTriangles/vector2.h"
#include "DelaunayTriangles/triangle.h"
#include "DelaunayTriangles/delaunay.h"

using namespace std;
//globals
const int TEXTURE_REPEAT = 8;

////////////////////////////////////////////////////////////////////////////////
// Class name: TerrainClass
////////////////////////////////////////////////////////////////////////////////
class TerrainClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	    D3DXVECTOR3 normal;
		float walkable = 0.0f;
	};

	struct VectorType
	{
		float x, y, z;
	};

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


	struct HeightMapType 
	{ 
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		VoronoiData *VorData;
		float walkable = 0.0f;
	};

	struct VoronoiRegion {
		vector<int> VRegionIndices;
		float maxDist = 0.0f;
		VoronoiPoint* vPoint;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	bool Initialize(ID3D11Device*, char*,WCHAR*);
	bool InitializeTerrain(ID3D11Device*, int terrainWidth, int terrainHeight, WCHAR*, WCHAR*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	bool RefreshTerrain(ID3D11Device * device, bool keydown);
	bool GenerateHeightMap(ID3D11Device* device, bool keydown);
	bool SmoothTerrain(ID3D11Device* device, bool keydown);
	void SmoothTerrain(int n);
	void Faulting();
	bool Faulting(ID3D11Device * device, bool keydown);
	void VoronoiRegions();
	void AddVoronoiPointAt(int index, int k);
	bool VoronoiRegions(ID3D11Device * device, bool keydown);
	void DelanuayTriangles();
	bool isCircular(vector<Edge*>& edges);
	bool isCircular(int v, bool visited[], vector<int, allocator<int>> **adj, int parent);	//This one is used for recursion
	void makeCorridors(const vector<Edge*> &tree);


	void PassThroughPerlinNoise();

	float RandomFloat(float a, float b);
	void Console();
	
	bool PassThroughPerlinNoise(ID3D11Device * device, bool keydown);
	bool PassProceduralFunction(ID3D11Device * device, bool keydown, bool (*x)());

	//int  GetIndexCount();

	ID3D11ShaderResourceView* GetGrassTexture();
	ID3D11ShaderResourceView* GetSlopeTexture();
	ID3D11ShaderResourceView* GetRockTexture();


	int GetVertexCount();
	void CopyVertexArray(void*);

private:
	bool LoadHeightMap(char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();

	void CalculateTextureCoordinates();
	bool LoadTexture(ID3D11Device*, WCHAR*,WCHAR*,WCHAR*);
	void ReleaseTexture();
	void ReleaseVornoi();


	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	//void RenderBuffers(ID3D11DeviceContext*);
	
private:
	bool m_terrainGeneratedToggle;
	int m_terrainWidth, m_terrainHeight;
	int m_vertexCount,m_indexCount;
	VertexType* m_vertices;
	HeightMapType* m_heightMap;
	TextureClass  *m_GrassTexture,*m_SlopeTexture, *m_RockTexture;
	vector<VoronoiRegion*> *m_VRegions;
	vector<VoronoiPoint*> *m_VPoints;
	vector<VoronoiRegion*> m_rooms;


};

#endif