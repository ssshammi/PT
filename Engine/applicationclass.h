////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "terrainclass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "fpsclass.h"
#include "cpuclass.h"
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
////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

private:
	bool HandleInput(float);
	bool RenderGraphics();
	bool Render();
	bool SetCameraMovement();

	bool InitializeBlurObjects(HWND, int screenWidth, int screenHeight);
	void ShutdownBlurObjects();
	bool RenderSceneToTexture();
	bool RenderCollectablesToTexture();
	bool RenderCollectablesOnly();
	bool Render2DTextureScene();
	bool UpSampleTexture();
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();
	bool RenderBloom1ToTexture();

private:
	D3DXMATRIX GetTransfromedMatrix(D3DXMATRIX worldMatrix);
	bool RenderText();


private:
	InputClass* m_Input;
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	TerrainClass* m_Terrain;
	TimerClass* m_Timer;
	PositionClass* m_Position;
	FpsClass* m_Fps;
	CpuClass* m_Cpu;
	FontShaderClass* m_FontShader;
	TextClass* m_Text;
	TerrainShaderClass* m_TerrainShader;
	LightClass* m_Light;
	FrustumClass* m_Frustum;
	QuadTreeClass* m_QuadTree;
	PointLightClass* m_PointLights[NUM_LIGHTS];
	GameManager* m_gameManager;
	bool m_freeCam, m_radialBlur;

	HorizontalBlurShaderClass* m_HorizontalBlurShader;
	HorizontalBlurShaderClass* m_VerticalBlurShader;
	BloomShader1Class* m_BloomShader1;
	RenderTextureClass *m_RenderTexture, *m_HorizontalBlurTexture, *m_VerticalBlurTexture, *m_Bloom1Texture, *m_UpSampleTexure;
	OrthoWindowClass *m_FullScreenWindow, *m_SmallWindow;
	TextureShaderClass* m_TextureShader;
	MultiplyShaderClass* m_MultiplyShader;
	
};

#endif