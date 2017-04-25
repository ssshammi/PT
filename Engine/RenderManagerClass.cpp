#include "RenderManagerClass.h"

RenderManagerClass::RenderManagerClass()
{
	//passed objects
	m_Direct3D = 0;
	m_Camera = 0;
	m_Text = 0;
	m_Light = 0;
	m_Terrain = 0;
	m_QuadTree = 0;
	m_gameManager = 0;

	for (int i = 0; i < NUM_LIGHTS; i++)
		m_PointLights[i] = 0;

	//generated objects
	m_FontShader = 0;
	m_TerrainShader = 0;
	m_Frustum = 0;

	//blur
	m_SmallWindow = 0;
	m_FullScreenWindow = 0;

	m_RenderTexture = 0;
	m_Bloom1Texture = 0;
	m_HorizontalBlurTexture = 0;
	m_VerticalBlurTexture = 0;
	m_UpSampleTexure = 0;
	m_radialBlurTexture = 0;

	m_HorizontalBlurShader = 0;
	m_VerticalBlurShader = 0;
	m_TextureShader = 0;
	m_RadialBlurShader = 0;

	m_bloomEnabled = true;
	m_radialBlur = true;
}

RenderManagerClass::RenderManagerClass(const RenderManagerClass &)
{
}

RenderManagerClass::~RenderManagerClass()
{
}


bool RenderManagerClass::Initialize(HWND hwnd, int screenWidth, int screenHeight, D3DClass* d3d, CameraClass* cam, TerrainClass* terrain, TextClass* text, QuadTreeClass* quadtree, LightClass* light, PointLightClass* pointlights[NUM_LIGHTS], GameManager* gm)
{
	bool result;
	//passing pointers from applcation class to this class
	m_Direct3D = d3d;
	if (!m_Direct3D)	return false;
	m_Camera = cam;
	if (!m_Camera)	return false;
	m_Text = text;
	if (!m_Text)	return false;
	m_Light = light;
	if (!m_Light)	return false;
	m_Terrain = terrain;
	if (!m_Terrain)	return false;
	m_QuadTree = quadtree;
	if (!m_QuadTree)	return false;
	m_gameManager = gm;
	if (!m_gameManager)	return false;

	for (int i = 0; i < NUM_LIGHTS; i++) {
		m_PointLights[i] = pointlights[i];
		if (!m_PointLights[i]) return false;
	}

	// Create the font shader object.
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	// Initialize the font shader object.
	result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}


	// Create the terrain shader object.
	m_TerrainShader = new TerrainShaderClass;
	if (!m_TerrainShader)
	{
		return false;
	}

	// Initialize the terrain shader object.
	result = m_TerrainShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the frustum object.
	m_Frustum = new FrustumClass;
	if (!m_Frustum)
	{
		return false;
	}




	// Initialize all the textures and windows required for the blur
	result = InitializeBlurObjects(hwnd, screenWidth, screenHeight);
	if (!result)	return false;


	return true;
}

void RenderManagerClass::Shutdown()
{
	ShutdownBlurObjects();

	m_Direct3D = 0;
	m_Camera = 0;
	m_Text = 0;
	m_Light = 0;
	m_Terrain = 0;
	m_QuadTree = 0;
	m_gameManager = 0;

	for (int i = 0; i < NUM_LIGHTS; i++)
		m_PointLights[i] = 0;


	// Release the terrain shader object.
	if (m_TerrainShader)
	{
		m_TerrainShader->Shutdown();
		delete m_TerrainShader;
		m_TerrainShader = 0;
	}

	// Release the frustum object.
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	// Release the font shader object.
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	return;
}

bool RenderManagerClass::InitializeBlurObjects(HWND hwnd, int screenWidth, int screenHeight)
{
	bool result;
	float downSampleWidth = screenWidth / 3, downSampleHeight = screenHeight / 3;

	//creating texture shader object
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)	return false;

	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result) return false;

	m_MultiplyShader = new MultiplyShaderClass;
	if (!m_MultiplyShader)	return false;

	result = m_MultiplyShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result) return false;

	// Create the horizontal blur shader object.
	m_HorizontalBlurShader = new HorizontalBlurShaderClass;
	if (!m_HorizontalBlurShader)
	{
		return false;
	}

	// Initialize the horizontal blur shader object.
	result = m_HorizontalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd, L"../Engine/horizontalblur.vs", L"../Engine/horizontalblur.ps");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the vertical blur shader object.
	m_VerticalBlurShader = new HorizontalBlurShaderClass;
	if (!m_VerticalBlurShader)
	{
		return false;
	}

	// Initialize the vertical blur shader object.
	result = m_VerticalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd, L"../Engine/verticalblur.vs", L"../Engine/verticalblur.ps");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the vertical blur shader object.
	m_RadialBlurShader = new RadialBlurShaderClass;
	if (!m_RadialBlurShader)
	{
		return false;
	}

	// Initialize the vertical blur shader object.
	result = m_RadialBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bloom blur shader object.", L"Error", MB_OK);
		return false;
	}


	// Create the render to texture object.
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the render to texture object.
	m_radialBlurTexture = new RenderTextureClass;
	if (!m_radialBlurTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_radialBlurTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the render to texture object.
	m_Bloom1Texture = new RenderTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_Bloom1Texture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bloom to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the horizontal blur render to texture object.
	m_HorizontalBlurTexture = new RenderTextureClass;
	if (!m_HorizontalBlurTexture)
	{
		return false;
	}

	// Initialize the horizontal blur render to texture object.
	result = m_HorizontalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the vertical blur render to texture object.
	m_VerticalBlurTexture = new RenderTextureClass;
	if (!m_VerticalBlurTexture)
	{
		return false;
	}

	// Initialize the vertical blur render to texture object.
	result = m_VerticalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the up sample render to texture object.
	m_UpSampleTexure = new RenderTextureClass;
	if (!m_UpSampleTexure)
	{
		return false;
	}

	// Initialize the up sample render to texture object.
	result = m_UpSampleTexure->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the up sample render to texture object.", L"Error", MB_OK);
		return false;
	}
	//Create and initialize the small 2D window model for down sampling and rendering the full scene render to texture onto.Use the down sample size as input.

	// Create the small ortho window object.
	m_SmallWindow = new OrthoWindowClass;
	if (!m_SmallWindow)
	{
		return false;
	}

	// Initialize the small ortho window object.
	result = m_SmallWindow->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the small ortho window object.", L"Error", MB_OK);
		return false;
	}



	// Create the full screen ortho window object.
	m_FullScreenWindow = new OrthoWindowClass;
	if (!m_FullScreenWindow)
	{
		return false;
	}

	// Initialize the full screen ortho window object.
	result = m_FullScreenWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
		return false;
	}

	return true;

}

void RenderManagerClass::ShutdownBlurObjects()
{
	// Release the small ortho window object.
	if (m_SmallWindow)
	{
		m_SmallWindow->Shutdown();
		delete m_SmallWindow;
		m_SmallWindow = 0;
	}

	if (m_UpSampleTexure)
	{
		m_UpSampleTexure->Shutdown();
		delete m_UpSampleTexure;
		m_UpSampleTexure = 0;
	}

	// Release the vertical blur render to texture object.
	if (m_VerticalBlurTexture)
	{
		m_VerticalBlurTexture->Shutdown();
		delete m_VerticalBlurTexture;
		m_VerticalBlurTexture = 0;
	}

	// Release the horizontal blur render to texture object.
	if (m_HorizontalBlurTexture)
	{
		m_HorizontalBlurTexture->Shutdown();
		delete m_HorizontalBlurTexture;
		m_HorizontalBlurTexture = 0;
	}


	if (m_radialBlurTexture)
	{
		m_radialBlurTexture->Shutdown();
		delete m_radialBlurTexture;
		m_radialBlurTexture = 0;
	}
	if (m_RadialBlurShader) {
		m_RadialBlurShader->Shutdown();
		delete m_RadialBlurShader;
		m_RadialBlurShader = 0;
	}
	if (m_MultiplyShader) {
		m_MultiplyShader->Shutdown();
		delete m_MultiplyShader;
		m_MultiplyShader = 0;
	}

	//Release texture shader
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the full screen ortho window object.
	if (m_FullScreenWindow)
	{
		m_FullScreenWindow->Shutdown();
		delete m_FullScreenWindow;
		m_FullScreenWindow = 0;
	}


	// Release the render to texture object.
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}
	if (m_VerticalBlurShader)
	{
		m_VerticalBlurShader->Shutdown();
		delete m_VerticalBlurShader;
		m_VerticalBlurShader = 0;
	}

	// Release the horizontal blur shader object.
	if (m_HorizontalBlurShader)
	{
		m_HorizontalBlurShader->Shutdown();
		delete m_HorizontalBlurShader;
		m_HorizontalBlurShader = 0;
	}


	return;
}


//render with blur
bool RenderManagerClass::Render()
{
	bool result;


	// First render the scene to a render texture.
	result = RenderSceneToTexture();
	if (!result)
	{
		return false;
	}

	// Next Render only the collectables to a new texture.
	result = RenderCollectablesToTexture();
	if (!result)
	{
		return false;
	}

	// Now perform a horizontal and vertical blur on the collectables texture. (This automatically downsizes)
	result = RenderHorizontalAndVerticalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// Up sample the final blurred render texture to screen size again.
	result = UpSampleTexture();
	if (!result)
	{
		return false;
	}

	//Combine the two textures (Blur and Rendered Texture) to a new texture for the next radial blur step
	result = RenderBlendCollectablesAndMainTexture();
	if (!result)	return false;


	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 0.0f);

	// Render the final texture to the screen and apply a radial blur simultaneously.
	result = Render2DTextureScene();
	if (!result)
	{
		return false;
	}

	// Render the text without the blur directly to the scene
	RenderText();

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}



bool RenderManagerClass::RenderGraphics()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool renderModel, result;
	int modelCount, renderCount, index;
	float positionX, positionY, positionZ, radius;
	D3DXVECTOR4 color;


	//obtain all point light colors and positions as an array to pass to the rendering functions
	D3DXVECTOR4 pointLightColors[NUM_LIGHTS];
	D3DXVECTOR4 pointLightPositions[NUM_LIGHTS];
	float pointLightRadius[NUM_LIGHTS], pointFallOutDist[NUM_LIGHTS];

	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		pointLightColors[i] = m_PointLights[i]->GetDiffuseColor();
		pointLightPositions[i] = m_PointLights[i]->GetPosition();
		pointLightRadius[i] = m_PointLights[i]->GetRadius();
		pointFallOutDist[i] = m_PointLights[i]->GetFallOffDistance();
	}

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);



	// Construct the frustum.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// Initialize the count of models that have been rendered.
	renderCount = 0;

	result = m_gameManager->Render(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Frustum,
		pointLightColors, pointLightPositions, pointLightRadius, pointFallOutDist, renderCount);
	if (!result) return false;





#pragma region RenderingQuadTerrain


	// Set the terrain shader parameters that it will use for rendering.
	result = m_TerrainShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Terrain->GetGrassTexture(), m_Terrain->GetSlopeTexture(), m_Terrain->GetRockTexture(), pointLightColors, pointLightPositions, pointLightRadius, pointFallOutDist);
	if (!result)
	{
		return false;
	}

	// Render the terrain using the quad tree and terrain shader.
	m_QuadTree->Render(m_Frustum, m_Direct3D->GetDeviceContext(), m_TerrainShader);




#pragma endregion

	return true;
}

bool RenderManagerClass::RenderText()
{
	bool result;
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Turn on the alpha blending before rendering the text.
	m_Direct3D->TurnOnAlphaBlending();

	// Render the text user interface elements.
	result = m_Text->Render(m_Direct3D->GetDeviceContext(), m_FontShader, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// Turn off alpha blending after rendering the text.
	m_Direct3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();
	return true;
}

bool RenderManagerClass::RenderSceneToTexture()
{
	bool result;
	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//render all the objects of the scene to the texture
	RenderGraphics();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_Direct3D->ResetViewport();

	return true;
}

bool RenderManagerClass::RenderCollectablesToTexture()
{
	bool result;
	// Set the render target to be the render to texture.
	m_Bloom1Texture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_Bloom1Texture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 0.0f);

	RenderCollectablesOnly();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_Direct3D->ResetViewport();

	return true;
}

bool RenderManagerClass::RenderCollectablesOnly() {

	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;
	int renderCount;


	//obtain all point light colors and positions as an array to pass to the rendering functions
	D3DXVECTOR4 pointLightColors[NUM_LIGHTS];
	D3DXVECTOR4 pointLightPositions[NUM_LIGHTS];
	float pointLightRadius[NUM_LIGHTS], pointFallOutDist[NUM_LIGHTS];

	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		pointLightColors[i] = m_PointLights[i]->GetDiffuseColor();
		pointLightPositions[i] = m_PointLights[i]->GetPosition();
		pointLightRadius[i] = m_PointLights[i]->GetRadius();
		pointFallOutDist[i] = m_PointLights[i]->GetFallOffDistance();
	}

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);


	//render all the objects of the scene to the texture
	result = m_gameManager->Render(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Frustum,
		pointLightColors, pointLightPositions, pointLightRadius, pointFallOutDist, renderCount);
	if (!result) return false;
	return true;
}

bool RenderManagerClass::Render2DTextureScene()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and ortho matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	D3DXMATRIX m = GetTransfromedMatrix(worldMatrix);

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	if (m_radialBlur) {
		// Render the full screen ortho window using the texture shader and the full screen sized blurred render to texture resource.
		result = m_RadialBlurShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), m, viewMatrix, orthoMatrix,
			m_radialBlurTexture->GetShaderResourceView());
	}
	else
	{
		// Render the full screen ortho window using the texture shader and the full screen sized blurred render to texture resource.
		result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), m, viewMatrix, orthoMatrix,
			m_radialBlurTexture->GetShaderResourceView());
	}
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();


	return true;
}

D3DXMATRIX RenderManagerClass::GetTransfromedMatrix(D3DXMATRIX worldMatrix) {

	D3DXMATRIX r, p, m = worldMatrix;
	D3DXVECTOR3 cam = m_Camera->GetPosition();
	D3DXVECTOR3 rot = m_Camera->GetRotation();

	D3DXMatrixTranslation(&m, 0, 0, 100);	//Adding some distance between the camera and the orthographic quad to render in
	D3DXMatrixRotationYawPitchRoll(&r, (rot.y)* 0.0174532925f, (rot.x)* 0.0174532925f, (rot.z)* 0.0174532925f);	//rotating the matrix according to the camera's rotation
	D3DXMatrixTranslation(&p, cam.x, cam.y, cam.z);	//moving the matrix to the camera's location
	D3DXMatrixMultiply(&m, &m, &r);			//multiplying the matrices
	D3DXMatrixMultiply(&m, &m, &p);

	return m;
}

bool RenderManagerClass::RenderBlendCollectablesAndMainTexture()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;

	// Set the render target to be the render to texture.
	m_radialBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_radialBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world and view matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	D3DXMATRIX m = GetTransfromedMatrix(worldMatrix);
	// Get the ortho matrix from the render to texture since texture has different dimensions.
	m_radialBlurTexture->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());


	// Turn on the alpha blending before rendering the text.
	m_Direct3D->TurnOnAlphaBlending();
	if (m_bloomEnabled)
		result = m_MultiplyShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), m, viewMatrix, orthoMatrix,
			m_RenderTexture->GetShaderResourceView(), m_UpSampleTexure->GetShaderResourceView());
	else
		result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), m, viewMatrix, orthoMatrix,
			m_RenderTexture->GetShaderResourceView());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_Direct3D->ResetViewport();

	return true;
}

bool RenderManagerClass::RenderHorizontalAndVerticalBlurToTexture()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeY, screenSizeX;
	bool result;

	// Store the screen width in a float that will be used in the horizontal blur shader.
	screenSizeX = (float)m_HorizontalBlurTexture->GetTextureWidth();

	// Set the render target to be the render to texture.
	m_HorizontalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_HorizontalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 0.5f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world and view matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	//transform world matrix to current camera location
	D3DXMATRIX m = GetTransfromedMatrix(worldMatrix);

	// Get the ortho matrix from the render to texture since texture has different dimensions.
	m_HorizontalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the small ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());
	// Turn on the alpha blending before rendering the text.
	m_Direct3D->TurnOnAlphaBlending();

	// Render the small ortho window using the horizontal blur shader and the down sampled render to texture resource.
	result = m_HorizontalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), m, viewMatrix, orthoMatrix,
		m_Bloom1Texture->GetShaderResourceView(), screenSizeX);


	// Store the screen height in a float that will be used in the vertical blur shader.
	screenSizeY = (float)m_VerticalBlurTexture->GetTextureHeight();

	// Set the render target to be the render to texture.
	m_VerticalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_VerticalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 0.5f);

	// Render the small ortho window using the vertical blur shader and the horizontal blurred render to texture resource.
	result = m_VerticalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), m, viewMatrix, orthoMatrix,
		m_HorizontalBlurTexture->GetShaderResourceView(), screenSizeY);
	if (!result)	return false;

	// Turn off alpha blending after rendering the text.
	m_Direct3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_Direct3D->ResetViewport();

	return true;
}

bool RenderManagerClass::UpSampleTexture()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;


	// Set the render target to be the render to texture.
	m_UpSampleTexure->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_UpSampleTexure->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world and view matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Get the ortho matrix from the render to texture since texture has different dimensions.
	m_UpSampleTexure->GetOrthoMatrix(orthoMatrix);


	D3DXMATRIX m = GetTransfromedMatrix(worldMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// Render the full screen ortho window using the texture shader and the small sized final blurred render to texture resource.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), m, viewMatrix, orthoMatrix,
		m_VerticalBlurTexture->GetShaderResourceView());
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_Direct3D->ResetViewport();

	return true;
}