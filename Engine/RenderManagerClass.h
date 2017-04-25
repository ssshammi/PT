#ifndef _RENDERMANAGERCLASS_H_
#define _RENDERMANAGERCLASS_H_
#include "d3dclass.h"
#include "cameraclass.h"
#include "terrainclass.h"
#include "fontshaderclass.h"
#include "textclass.h"
#include "terrainshaderclass.h"
#include "lightclass.h"
#include "frustumclass.h"
#include "quadtreeclass.h"
#include "pointlightclass.h"
#include "GameManager.h"
//for blurring effect
#include "orthowindowclass.h"
#include "rendertextureclass.h"
#include "textureshaderclass.h"
#include "horizontalblurshaderclass.h"
#include "BloomShader1Class.h"
#include "MultiplyShaderClass.h"
#include "radialBlurShader.h"

////GLOBALS

const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

////Class
class RenderManagerClass {
public:
	RenderManagerClass();
	RenderManagerClass(const RenderManagerClass&);
	~RenderManagerClass();

	bool Initialize(HWND hwnd, int screenWidth, int screenHeight, D3DClass* d3d, CameraClass* cam, TerrainClass* terrain, TextClass* text, QuadTreeClass* quadtree, LightClass* light, PointLightClass* pointlights[NUM_LIGHTS], GameManager* gm);
	void Shutdown();
	bool Render();

protected:
	bool InitializeBlurObjects(HWND, int screenWidth, int screenHeight);
	void ShutdownBlurObjects();
	bool RenderGraphics();
	bool RenderSceneToTexture();
	bool RenderCollectablesToTexture();
	bool RenderCollectablesOnly();
	bool Render2DTextureScene();
	bool UpSampleTexture();
	bool RenderHorizontalAndVerticalBlurToTexture();
	bool RenderBlendCollectablesAndMainTexture();

private:
	D3DXMATRIX GetTransfromedMatrix(D3DXMATRIX worldMatrix);
	bool RenderText();


private:
	//Transfered objects from application class
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	TextClass* m_Text;
	LightClass* m_Light;
	PointLightClass* m_PointLights[NUM_LIGHTS];
	TerrainClass* m_Terrain;
	QuadTreeClass* m_QuadTree;
	GameManager* m_gameManager;

private:
	//created objects
	FrustumClass* m_Frustum;
	TerrainShaderClass* m_TerrainShader;
	FontShaderClass* m_FontShader;


	HorizontalBlurShaderClass* m_HorizontalBlurShader;
	HorizontalBlurShaderClass* m_VerticalBlurShader;
	RadialBlurShaderClass* m_RadialBlurShader;
	RenderTextureClass *m_RenderTexture, *m_HorizontalBlurTexture, *m_VerticalBlurTexture, *m_Bloom1Texture, *m_UpSampleTexure, *m_radialBlurTexture;
	OrthoWindowClass *m_FullScreenWindow, *m_SmallWindow;
	TextureShaderClass* m_TextureShader;
	MultiplyShaderClass* m_MultiplyShader;

public:
	bool m_radialBlur, m_bloomEnabled;
};


#endif