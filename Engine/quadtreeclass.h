////////////////////////////////////////////////////////////////////////////////
// Filename: quadtreeclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _QUADTREECLASS_H_
#define _QUADTREECLASS_H_

/////////////
// GLOBALS //
/////////////
const int MAX_TRIANGLES = 10000;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "terrainclass.h"
#include "frustumclass.h"
#include "terrainshaderclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: QuadTreeClass
////////////////////////////////////////////////////////////////////////////////
class QuadTreeClass
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
		float walkable = 0.0f;
	};

	struct NodeType
	{
		float positionX, positionZ, width;
		int triangleCount;
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		NodeType* nodes[4];
		VectorType* vertexArray;
		int* mainTextureIndex;
	};

public:
	QuadTreeClass();
	QuadTreeClass(const QuadTreeClass&);
	~QuadTreeClass();

	bool Initialize(TerrainClass*, ID3D11Device*);
	void Shutdown();
	void Render(FrustumClass*, ID3D11DeviceContext*, TerrainShaderClass*);
	void ReinitializeBuffers(TerrainClass*,ID3D11Device* device);
	int GetDrawCount();
	bool GetHeightAtPosition(float, float, float&, bool& canWalk);


private:
	void CalculateMeshDimensions(int, float&, float&, float&);
	void CreateTreeNode(NodeType*, float, float, float, ID3D11Device*);
	int CountTriangles(float, float, float);
	bool IsTriangleContained(int, float, float, float);
	void ResetNodeBuffers(NodeType*, ID3D11Device*);
	void ReleaseNode(NodeType*);
	void RenderNode(NodeType*, FrustumClass*, ID3D11DeviceContext*, TerrainShaderClass*);
	void FindNode(NodeType*, float, float, float&,bool& canWalk);
	bool CheckHeightOfTriangle(float, float, float&, float[3], float[3], float[3]);


private:
	int m_triangleCount, m_drawCount;

	VertexType* m_vertexList;

	NodeType* m_parentNode;
};

#endif