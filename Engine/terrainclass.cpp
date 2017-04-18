////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "terrainclass.h"
#include <cmath>
#include "PerlinNoise.h"
#include <iostream>
#include <Windows.h>
#include <algorithm>
#include "textclass.h"



TerrainClass::TerrainClass()
{
	m_heightMap = 0;
	m_terrainGeneratedToggle = false;
	m_GrassTexture = 0;
	m_SlopeTexture= 0;
	m_RockTexture = 0;
	m_vertices = 0;
	m_vornoi = 0;
}


TerrainClass::TerrainClass(const TerrainClass& other)
{
}


TerrainClass::~TerrainClass()
{
}

bool TerrainClass::InitializeTerrain(ID3D11Device* device, int terrainWidth, int terrainHeight, WCHAR* GrassTextureFilename, WCHAR* SlopeTextureFilename, WCHAR* RockTextureFilename)
{
	int index;
	float height = 0.0;
	bool result;

	// Save the dimensions of the terrain.
	m_terrainWidth = terrainWidth;
	m_terrainHeight = terrainHeight;

	// Create the structure to hold the terrain data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if(!m_heightMap)
	{
		return false;
	}

	// Initialise the data in the height map (flat).
	for(int j=0; j<m_terrainHeight; j++)
	{
		for(int i=0; i<m_terrainWidth; i++)
		{			
			index = (m_terrainHeight * j) + i;

			float i1 = (float)i;
			float j1 = (float)j;
			//i1 *= 0.25f;
			//j1 *= 0.25f;
			/*if (i% 2 == 0)
				j1 += 0.5f;*/

			m_heightMap[index].x = i1;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = j1;

		}
	}

	RunProceduralFunctions();

	//even though we are generating a flat terrain, we still need to normalise it. 
	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if(!result)
	{
		return false;
	}

	//Calculate the Texture
	CalculateTextureCoordinates();

	//load the texture
	result = LoadTexture(device, GrassTextureFilename,SlopeTextureFilename,RockTextureFilename);
	if (!result)
	{
		return false;
	}


	// Initialize the vertex array that will hold the geometry for the terrain.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	return true;
}

void TerrainClass::RunProceduralFunctions() {

	PassThroughPerlinNoise();

	for (int i = 0; i < 45; i++) 
		Faulting();

	for (int i = 0; i < 4; i++)
		AddRandomNoise();	


	SmoothTerrain(20);
	
	VoronoiRegions();
}


bool TerrainClass::Initialize(ID3D11Device* device, char* heightMapFilename, WCHAR* textureFilename)
{
	bool result;


	// Load in the height map for the terrain.
	result = LoadHeightMap(heightMapFilename);
	if(!result)
	{
		return false;
	}

	// Normalize the height of the height map.
	NormalizeHeightMap();

	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if(!result)
	{
		return false;
	}

	//Calculate the Texture
	CalculateTextureCoordinates();

	//load the texture
	//result = LoadTexture(device,textureFilename);
	if (!result)
	{
		return false;
	}


	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	return true;
}

ID3D11ShaderResourceView* TerrainClass::GetGrassTexture() {
	return m_GrassTexture->GetTexture();
}

ID3D11ShaderResourceView* TerrainClass::GetSlopeTexture() {
	return m_SlopeTexture->GetTexture();
}


ID3D11ShaderResourceView* TerrainClass::GetRockTexture() {
	return m_RockTexture->GetTexture();
}

void TerrainClass::Shutdown()
{
	if (m_vornoi) {
		m_vornoi->ReleaseVornoi();
		m_vornoi = 0;
	}

	if (!m_rooms.empty()) {	m_rooms.clear();}

	if (!m_corridors.empty()) 
	{
		for (vector<vector<HeightMapType*>>::iterator i = m_corridors.begin(); i != m_corridors.end(); ++i) 
			(*i).clear();

		m_corridors.clear();
	}
	//release texture
	ReleaseTexture();

	// Release the vertex array.
	ShutdownBuffers();

	// Release the height map data.
	ShutdownHeightMap();

	return;
}


void TerrainClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//RenderBuffers(deviceContext);

	return;
}


bool TerrainClass::RefreshTerrain(ID3D11Device* device, bool keydown)
{
	bool result;
	//the toggle is just a bool that I use to make sure this is only called ONCE when you press a key
	//until you release the key and start again. We dont want to be generating the terrain 500
	//times per second. 
	if (keydown)
	{
		int index = 0;
		for (int j = 0; j<m_terrainHeight; j++)
		{
			for (int i = 0; i<m_terrainWidth; i++)
			{
				index = (m_terrainHeight * j) + i;

				//m_heightMap[index].x = (float)i;
				m_heightMap[index].y = 0.0f;
				//m_heightMap[index].z = (float)j;
				m_heightMap[index].walkable = 0.0f;
			}
		}

		result = CalculateNormals();
		if (!result){return false;}

		// Initialize the vertex and index buffer that hold the geometry for the terrain.
		result = InitializeBuffers(device);
		if (!result){return false;}

		m_terrainGeneratedToggle = true;
	}
	else
	{
		m_terrainGeneratedToggle = false;
	}
	return true;
}


void TerrainClass::AddRandomNoise() {
	int index;
	float height = 0.0;


	//loop through the terrain and set the hieghts how we want. This is where we generate the terrain
	//in this case I will run a sin-wave through the terrain in one axis.

	for (int j = 0; j<m_terrainHeight; j++)
	{
		for (int i = 0; i<m_terrainWidth; i++)
		{
			index = (m_terrainHeight * j) + i;

			m_heightMap[index].y = m_heightMap[index].y + RandomFloat(-0.4f, 0.4f);//(float)((rand()%15));//(float)(cos((float)i/(m_terrainWidth/12))*3.0); //magic numbers ahoy, just to ramp up the height of the sin function so its visible.
		}
	}

}

bool TerrainClass::AddRandomNoise(ID3D11Device* device, bool keydown)
{

	bool result;
	//the toggle is just a bool that I use to make sure this is only called ONCE when you press a key
	//until you release the key and start again. We dont want to be generating the terrain 500
	//times per second. 
	if(keydown&&(!m_terrainGeneratedToggle))
	{
		AddRandomNoise();

		result = CalculateNormals();
		if(!result)
		{
			return false;
		}

		// Initialize the vertex and index buffer that hold the geometry for the terrain.
		result = InitializeBuffers(device);
		if(!result)
		{
			return false;
		}

		m_terrainGeneratedToggle = true;
	}
	else
	{
		m_terrainGeneratedToggle = false;
	}

	return true;
}


void TerrainClass::SmoothTerrain(int n = 1) {

	int index;
	float height = 0.0;


	//smoothing
	for (int num = 0; num <n; num++) {
		for (int j = 0; j < m_terrainHeight; j++)
		{
			for (int i = 0; i < m_terrainWidth; i++)
			{

				//std::cout << "ASDASD" << std::endl;
				index = (m_terrainHeight * j) + i;
				int index2 = (m_terrainHeight*(j + 1)) + i;
				int index3 = (m_terrainHeight*(j - 1)) + i;
				int iNeg = -1;
				int iPos = 1;


				//m_heightMap[index].x = (float)i;
				

				if (j == 0) {
					index3 = index;
				}
				else if (j == m_terrainHeight - 1) {
					index2 = index;
				}

				if (i == 0) {
					iNeg = 0;
				}
				else if (i == m_terrainWidth - 1)
					iPos = 0;

				m_heightMap[index].y = (float)((m_heightMap[index + iPos].y + m_heightMap[index + iNeg].y + m_heightMap[index2 + iPos].y + m_heightMap[index2 +iNeg].y + m_heightMap[index3 + iPos].y + m_heightMap[index3 + iNeg].y + m_heightMap[index3].y + m_heightMap[index2].y) / 8);
			}
		}
	}


}

bool TerrainClass::SmoothTerrain(ID3D11Device* device, bool keydown)
{

	bool result;
	//the toggle is just a bool that I use to make sure this is only called ONCE when you press a key
	//until you release the key and start again. We dont want to be generating the terrain 500
	//times per second. 
	if (keydown)
	{

		SmoothTerrain(1);

		result = CalculateNormals();
		if (!result)
		{
			return false;
		}

		// Initialize the vertex and index buffer that hold the geometry for the terrain.
		result = InitializeBuffers(device);
		if (!result)
		{
			return false;
		}

		m_terrainGeneratedToggle = true;
	}
	else
	{
		m_terrainGeneratedToggle = false;
	}
	return true;
}



void TerrainClass::Faulting()
{
	int x1, y1, x2, y2;
	float m, b;
		x1 = (int)m_terrainWidth*0.1f  + (int)(rand() % (int)(m_terrainWidth*0.8f));
		y1 = (rand()%2 == 0)? m_terrainHeight-1:0;
		int random1 = 0;
		while (random1 == 0 || x2 == x1) {
			random1 = (int)RandomFloat(-5.0f, 5.0f);
			x2 = x1 + random1;		//random between -15 and 15
		}
		int random2 = 0;
		while (random2 == 0)
			random2 = (int)RandomFloat(-5.0f, 5.0f);//(25 - (rand() % 50));
		y2 = y1 + random2;		//random between -15 and 15

		m = ((float)(y2 - y1)) / ((float)(x2 - x1));
		b = (float)y1 - (((float)x1)*m);


		float H1 = RandomFloat(-2.0f,2.0f);
		
		float H2 = H1*0.5f;
	
	//Console();
	int index;
	//BOOL WINAPI AllocConsole(void);



	//Faulting
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainWidth * j) + i;

			bool eq = (float)j > ((float)i*m)+b;
			if (eq) 
				m_heightMap[index].y += H1;

			m_heightMap[index].y -= H1;

		}
	}

}

float TerrainClass::RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}


bool TerrainClass::Faulting(ID3D11Device* device, bool keydown)
{
	bool result;
	//the toggle is just a bool that I use to make sure this is only called ONCE when you press a key
	//until you release the key and start again. We dont want to be generating the terrain 500
	//times per second. 
	if (keydown)
	{

		Faulting();

		result = CalculateNormals();
		if (!result)
		{
			return false;
		}

		// Initialize the vertex and index buffer that hold the geometry for the terrain.
		result = InitializeBuffers(device);
		if (!result)
		{
			return false;
		}

		m_terrainGeneratedToggle = true;
	}
	else
	{
		m_terrainGeneratedToggle = false;
	}
	return true;
}


bool TerrainClass::VoronoiRegions(ID3D11Device* device, bool keydown)
{
	bool result;
	//the toggle is just a bool that I use to make sure this is only called ONCE when you press a key
	//until you release the key and start again. We dont want to be generating the terrain 500
	//times per second. 
	if (keydown)
	{

		VoronoiRegions();

		result = CalculateNormals();
		if (!result)
		{
			return false;
		}

		// Initialize the vertex and index buffer that hold the geometry for the terrain.
		result = InitializeBuffers(device);
		if (!result)
		{
			return false;
		}

		m_terrainGeneratedToggle = true;
	}
	else
	{
		m_terrainGeneratedToggle = false;
	}
	return true;
}

void TerrainClass::VoronoiRegions()
{
	m_vornoi = new Vornoi();
	m_vornoi->VoronoiRegions(m_heightMap,m_terrainWidth,m_terrainHeight,m_rooms,m_corridors);
	m_heightMap[m_rooms.at(0)->vPoint->index].y = 20.0f;
}



void TerrainClass::PassThroughPerlinNoise()
{

	PerlinNoise::initialize();
	//Console();
	int index;
	//BOOL WINAPI AllocConsole(void);

	//smoothing
	for (int j = 0; j < m_terrainHeight; j++)
	{
		for (int i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainHeight * j) + i;
			double f = PerlinNoise::noise((double)j/10 , (double)i/10 , 1);
			m_heightMap[index].y += f;
			//std::cout << f << std::endl;
			//printf("This is:  %e \n",f);
		}
	}

	PerlinNoise::Release();
	
}

bool TerrainClass::PassThroughPerlinNoise(ID3D11Device* device, bool keydown)
{

	bool result;
	//the toggle is just a bool that I use to make sure this is only called ONCE when you press a key
	//until you release the key and start again. We dont want to be generating the terrain 500
	//times per second. 
	if (keydown)
	{

		PassThroughPerlinNoise();

		result = CalculateNormals();
		if (!result)
		{
			return false;
		}

		// Initialize the vertex and index buffer that hold the geometry for the terrain.
		result = InitializeBuffers(device);
		if (!result)
		{
			return false;
		}

		m_terrainGeneratedToggle = true;
	}
	else
	{
		m_terrainGeneratedToggle = false;
	}
	return true;
}

bool TerrainClass::PassProceduralFunction(ID3D11Device* device, bool keydown, bool (*x)())
{

	bool result;
	//the toggle is just a bool that I use to make sure this is only called ONCE when you press a key
	//until you release the key and start again. We dont want to be generating the terrain 500
	//times per second. 
	if (keydown)
	{

		x();

		result = CalculateNormals();
		if (!result)
		{
			return false;
		}

		// Initialize the vertex and index buffer that hold the geometry for the terrain.
		result = InitializeBuffers(device);
		if (!result)
		{
			return false;
		}

		m_terrainGeneratedToggle = true;
	}
	else
	{
		m_terrainGeneratedToggle = false;
	}
	return true;
}

void TerrainClass::Console()
{
	AllocConsole();
	FILE *pFileCon = NULL;
	pFileCon = freopen("CONOUT$", "w", stdout);

	COORD coordInfo;
	coordInfo.X = 130;
	coordInfo.Y = 9000;

	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coordInfo);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS);
}

bool TerrainClass::LoadHeightMap(char* filename)
{
	FILE* filePtr;
	int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize, i, j, k, index;
	unsigned char* bitmapImage;
	unsigned char height;


	// Open the height map file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Save the dimensions of the terrain.
	m_terrainWidth = bitmapInfoHeader.biWidth;
	m_terrainHeight = bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data.
	imageSize = m_terrainWidth * m_terrainHeight * 3;

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if(!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Create the structure to hold the height map data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if(!m_heightMap)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k=0;

	// Read the image data into the height map.
	for(j=0; j<m_terrainHeight; j++)
	{
		for(i=0; i<m_terrainWidth; i++)
		{
			height = bitmapImage[k];
			
			index = (m_terrainHeight * j) + i;

			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = (float)j;

			k+=3;
		}
	}

	// Release the bitmap image data.
	delete [] bitmapImage;
	bitmapImage = 0;

	return true;
}


void TerrainClass::NormalizeHeightMap()
{
	int i, j;


	for(j=0; j<m_terrainHeight; j++)
	{
		for(i=0; i<m_terrainWidth; i++)
		{
			m_heightMap[(m_terrainHeight * j) + i].y /= 15.0f;
		}
	}

	return;
}


bool TerrainClass::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;


	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new VectorType[(m_terrainHeight-1) * (m_terrainWidth-1)];
	if(!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for(j=0; j<(m_terrainHeight-1); j++)
	{
		for(i=0; i<(m_terrainWidth-1); i++)
		{
			index1 = (j * m_terrainHeight) + i;
			index2 = (j * m_terrainHeight) + (i+1);
			index3 = ((j+1) * m_terrainHeight) + i;

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;
		
			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;
		
			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainHeight-1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for(j=0; j<m_terrainHeight; j++)
	{
		for(i=0; i<m_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if(((i-1) >= 0) && ((j-1) >= 0))
			{
				index = ((j-1) * (m_terrainHeight-1)) + (i-1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if((i < (m_terrainWidth-1)) && ((j-1) >= 0))
			{
				index = ((j-1) * (m_terrainHeight-1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if(((i-1) >= 0) && (j < (m_terrainHeight-1)))
			{
				index = (j * (m_terrainHeight-1)) + (i-1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if((i < (m_terrainWidth-1)) && (j < (m_terrainHeight-1)))
			{
				index = (j * (m_terrainHeight-1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}
			
			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));
			
			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainHeight) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete [] normals;
	normals = 0;

	return true;
}





void TerrainClass::ShutdownHeightMap()
{
	if(m_heightMap)
	{
		int size = (int) sizeof(m_heightMap) / sizeof(m_heightMap[0]);
		for (int i = 0; i < size; i++) {
			delete m_heightMap[i].VorData;
			m_heightMap[i].VorData = 0;
		}

		delete [] m_heightMap;
		m_heightMap = 0;
	}

	return;
}


void TerrainClass::CalculateTextureCoordinates()
{
	int incrementCount, i, j, tuCount, tvCount;
	float incrementValue, tuCoordinate, tvCoordinate;


	// Calculate how much to increment the texture coordinates by.
	incrementValue = (float)TEXTURE_REPEAT / (float)m_terrainWidth;

	// Calculate how many times to repeat the texture.
	incrementCount = m_terrainWidth / TEXTURE_REPEAT;

	// Initialize the tu and tv coordinate values.
	tuCoordinate = 0.0f;
	tvCoordinate = 1.0f;

	// Initialize the tu and tv coordinate indexes.
	tuCount = 0;
	tvCount = 0;

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for (j = 0; j<m_terrainHeight; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			// Store the texture coordinate in the height map.
			m_heightMap[(m_terrainHeight * j) + i].tu = tuCoordinate;
			m_heightMap[(m_terrainHeight * j) + i].tv = tvCoordinate;

			// Increment the tu texture coordinate by the increment value and increment the index by one.
			tuCoordinate += incrementValue;
			tuCount++;

			// Check if at the far right end of the texture and if so then start at the beginning again.
			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// Increment the tv texture coordinate by the increment value and increment the index by one.
		tvCoordinate -= incrementValue;
		tvCount++;

		// Check if at the top of the texture and if so then start at the bottom again.
		if (tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}

	return;
}

bool TerrainClass::LoadTexture(ID3D11Device* device, WCHAR* GrassTextureFilename, WCHAR* SlopeTextureFilename,WCHAR* RockTextureFilename)
{
	bool result;


	// Create the texture object.
	m_GrassTexture = new TextureClass;
	if (!m_GrassTexture)
	{
		return false;
	}
	m_SlopeTexture = new TextureClass;
	if (!m_SlopeTexture)
	{
		return false;
	}
	m_RockTexture = new TextureClass;
	if (!m_RockTexture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_GrassTexture->Initialize(device, GrassTextureFilename);
	if (!result)
	{
		return false;
	}

	result = m_SlopeTexture->Initialize(device, SlopeTextureFilename);
	if (!result)
	{
		return false;
	}
	result = m_RockTexture->Initialize(device, RockTextureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}
//The ReleaseTexture function releases the TextureClass object.

void TerrainClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_GrassTexture)
	{
		m_GrassTexture->Shutdown();
		delete m_GrassTexture;
		m_GrassTexture = 0;
	}

	// Release the texture object.
	if (m_SlopeTexture)
	{
		m_SlopeTexture->Shutdown();
		delete m_SlopeTexture;
		m_SlopeTexture = 0;
	}

	// Release the texture object.
	if (m_RockTexture)
	{
		m_RockTexture->Shutdown();
		delete m_RockTexture;
		m_RockTexture = 0;
	}
	return;
}



bool TerrainClass::InitializeBuffers(ID3D11Device* device)
{
	int index, i, j;
	int index1, index2, index3, index4;
	float tu, tv;


	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	m_vertices = new VertexType[m_vertexCount];
	if(!m_vertices)
	{
		return false;
	}

	// Initialize the index to the vertex buffer.
	index = 0;

	// Load the vertex and index array with the terrain data.
	// Load the vertex and index array with the terrain data.
	for (j = 0; j<(m_terrainHeight - 1); j++)
	{
		for (i = 0; i<(m_terrainWidth - 1); i++)
		{
			bool straight = (i % 2 == j%2);
			//altering the triangles
			if (straight) {
				index1 = (m_terrainHeight * j) + i;          // Bottom left.
				index2 = (m_terrainHeight * j) + (i + 1);      // Bottom right.
				index3 = (m_terrainHeight * (j + 1)) + i;      // Upper left.
				index4 = (m_terrainHeight * (j + 1)) + (i + 1);  // Upper right.

				tv = m_heightMap[index3].tv;

				// Modify the texture coordinates to cover the top edge.
				if (tv == 1.0f) { tv = 0.0f; }

				m_vertices[index].position = D3DXVECTOR3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
				m_vertices[index].texture = D3DXVECTOR2(m_heightMap[index3].tu, tv);
				m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
				m_vertices[index].walkable = m_heightMap[index3].walkable;
				index++;

				// Upper right.
				tu = m_heightMap[index4].tu;
				tv = m_heightMap[index4].tv;

				// Modify the texture coordinates to cover the top and right edge.
				if (tu == 0.0f) { tu = 1.0f; }
				if (tv == 1.0f) { tv = 0.0f; }

				m_vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
				m_vertices[index].texture = D3DXVECTOR2(tu, tv);
				m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
				m_vertices[index].walkable = m_heightMap[index4].walkable;
				index++;

				// Bottom left.
				m_vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
				m_vertices[index].texture = D3DXVECTOR2(m_heightMap[index1].tu, m_heightMap[index1].tv);
				m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
				m_vertices[index].walkable = m_heightMap[index1].walkable;
				index++;

				// Bottom left.
				m_vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
				m_vertices[index].texture = D3DXVECTOR2(m_heightMap[index1].tu, m_heightMap[index1].tv);
				m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
				m_vertices[index].walkable = m_heightMap[index1].walkable;
				index++;

				// Upper right.
				tu = m_heightMap[index4].tu;
				tv = m_heightMap[index4].tv;

				// Modify the texture coordinates to cover the top and right edge.
				if (tu == 0.0f) { tu = 1.0f; }
				if (tv == 1.0f) { tv = 0.0f; }

				m_vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
				m_vertices[index].texture = D3DXVECTOR2(tu, tv);
				m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
				m_vertices[index].walkable = m_heightMap[index4].walkable;
				index++;

				// Bottom right.
				tu = m_heightMap[index2].tu;

				// Modify the texture coordinates to cover the right edge.
				if (tu == 0.0f) { tu = 1.0f; }

				m_vertices[index].position = D3DXVECTOR3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
				m_vertices[index].texture = D3DXVECTOR2(tu, m_heightMap[index2].tv);
				m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
				m_vertices[index].walkable = m_heightMap[index2].walkable;
				index++;
			}


			else {
				index1 = (m_terrainHeight * j) + (i + 1);        // Bottom right.
				index2 = (m_terrainHeight * (j + 1)) + (i + 1);  // Upper right.   
				index3 = (m_terrainHeight * j) + i;			     // Bottom left.
				index4 = (m_terrainHeight * (j + 1)) + i;        //  Upper left.
				tv = m_heightMap[index3].tv;


				m_vertices[index].position = D3DXVECTOR3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
				m_vertices[index].texture = D3DXVECTOR2(m_heightMap[index3].tu, m_heightMap[index3].tv);
				m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
				m_vertices[index].walkable = m_heightMap[index3].walkable;
				index++;

				
				tv = m_heightMap[index4].tv;

				// Modify the texture coordinates to cover the top edge.
				if (tv == 1.0f) { tv = 0.0f; }

				m_vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
				m_vertices[index].texture = D3DXVECTOR2(m_heightMap[index4].tu, tv);
				m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
				m_vertices[index].walkable = m_heightMap[index4].walkable;
				index++;

				// Bottom right.
				tu = m_heightMap[index1].tu;

				// Modify the texture coordinates to cover the right edge.
				if (tu == 0.0f) { tu = 1.0f; }

				m_vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
				m_vertices[index].texture = D3DXVECTOR2(tu, m_heightMap[index1].tv);
				m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
				m_vertices[index].walkable = m_heightMap[index1].walkable;
				index++;

				// Bottom left.
				m_vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
				m_vertices[index].texture = D3DXVECTOR2(tu, m_heightMap[index1].tv);
				m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
				m_vertices[index].walkable = m_heightMap[index1].walkable;
				index++;

				//upper left
				tv = m_heightMap[index4].tv;

				// Modify the texture coordinates to cover the top edge.
				if (tv == 1.0f) { tv = 0.0f; }

				m_vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
				m_vertices[index].texture = D3DXVECTOR2(m_heightMap[index4].tu, tv);
				m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
				m_vertices[index].walkable = m_heightMap[index4].walkable;
				index++;

				// Upper right.
				tu = m_heightMap[index2].tu;
				tv = m_heightMap[index2].tv;

				// Modify the texture coordinates to cover the top and right edge.
				if (tu == 0.0f) { tu = 1.0f; }
				if (tv == 1.0f) { tv = 0.0f; }

				m_vertices[index].position = D3DXVECTOR3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
				m_vertices[index].texture = D3DXVECTOR2(tu, tv);
				m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
				m_vertices[index].walkable = m_heightMap[index2].walkable;
				index++;
			}


															 // Upper left.
			
		}
	}
	return true;
}


void TerrainClass::ShutdownBuffers()
{
	// Release the vertex array.
	if (m_vertices)
	{
		delete[] m_vertices;
		m_vertices = 0;
	}


	return;
}

void TerrainClass::CopyVertexArray(void* vertexList)
{
	memcpy(vertexList, m_vertices, sizeof(VertexType) * m_vertexCount);
	return;
}

bool TerrainClass::GetPlayerStart(float & x, float & y, float & z)
{
	if (!m_rooms[0]) {
		return false;
	}

	x = m_rooms[0]->vPoint->x;
	y = m_rooms[0]->vPoint->y;
	z = m_rooms[0]->vPoint->z;
	return true;
}
D3DXVECTOR3 TerrainClass::GetPlayerStart()
{
	if (!m_rooms[0]) {
		return D3DXVECTOR3(0,0,0);
	}
	D3DXVECTOR3 v;
	v.x = m_rooms[0]->vPoint->x;
	v.y = m_rooms[0]->vPoint->y;
	v.z = m_rooms[0]->vPoint->z;
	return v;
}

bool TerrainClass::GetCollectablePoints( vector<D3DXVECTOR3>  &vc, int N)		//N is the number of collectables
{
	int nC = m_corridors.size();	//total number of corridors
	int nR = m_rooms.size();		//total number of rooms
	int pR = RandomFloat(0,N/2);	//Getting a certain number of collectables to be foudn in the rooms
	int pC = N - pR;				//getting a certain number of collectables to be foudn in the corridors
	int *a = new int [pR];			//creating an array to hold the index of a random room
	int *b = new int[pC];			//creating an array to hold the index of a random column

	//finding the points in the rooms
	for (int i = 0; i < pR; i++) {
		a[i] = RandomFloat(1,nR-1);	//0 is not used as the player is spawned at 0
		bool repeated = false;
		//ensuring that the index obtained above is not repeated
		for (int j = i - 1; j >= 0; j--) {
			if (a[i] == a[j]) {
				i--;
				repeated = true;
				break;
			}
		}
		//adding the co-ordinates to the given vector
		if (!repeated) {
			D3DXVECTOR3 vec;
			vec.x = m_rooms[a[i]]->vPoint->x;
			vec.y = m_rooms[a[i]]->vPoint->y - m_rooms[a[i]]->vPoint->height +1.0f  ;
			vec.z = m_rooms[a[i]]->vPoint->z;
			vc.push_back(vec);
		}
	}


	//finding the points in the corridors
	for (int i = 0; i < pC; i++) {
		b[i] = RandomFloat(0, nC - 1);
		bool repeated = false;
		//ensuring that the index obtained above is not repeated
		for (int j = i - 1; j >= 0; j--) {
			if (b[i] == b[j]) {
				i--;
				repeated = true;
				break;
			}
		}
		//adding the unique co-ordinates to the given vector
		if (!repeated) {
			D3DXVECTOR3 vec;
			//getting the point in the midipoint of the given random corridor
			HeightMapType* h = (m_corridors[b[i]] [m_corridors[b[i]].size()/2]);

			vec.x = h->x;
			vec.y = h->y + 1.0f;
			vec.z = h->z;
			vc.push_back(vec);

			h = 0;
		}
	}


	delete[]a;
	a = 0;
	delete[] b;
	b = 0;

	return true;
}

int TerrainClass::GetVertexCount()
{
	return m_vertexCount;
}
