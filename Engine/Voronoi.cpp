#include "Voronoi.h"
#include <algorithm>
#include <ctime>
#include "PerlinNoise.h"
#include <iostream>

void Vornoi::AddVoronoiPointAt(int IndexInArray, int RegionIndex) {

	//creating a voronoi seed and the definitio of a voronoi region for each voronoi seed
	VoronoiPoint* v = new VoronoiPoint;
	VoronoiRegion* r = new VoronoiRegion;
	v->x = m_heightMap[IndexInArray].x;
	v->y = m_heightMap[IndexInArray].y;
	v->z = m_heightMap[IndexInArray].z;
	v->index = IndexInArray;
	v->height = RegionIndex;
	if (RegionIndex % 2 == 0)
		v->height = -v->height;
	v->RegionIndex = RegionIndex;
	r->vPoint = v;
	m_VPoints->push_back(v);
	m_VRegions->push_back(r);
	m_VRegions->at(RegionIndex)->maxDist = 0.0f;
	v = 0;
	r = 0;

}

Vornoi::Vornoi()
{
	m_VPoints = 0;
	m_VRegions = 0;
}

Vornoi::Vornoi(const Vornoi &)
{
}

Vornoi::~Vornoi()
{
}


void Vornoi::GenerateVoronoiDungeon(HeightMapType *hmap, int terrainWidth, int terrainHeight, vector<VoronoiRegion*> &rooms, vector<vector<HeightMapType*>> &corridors) {
	m_heightMap = hmap;
	m_terrainWidth = terrainWidth;
	m_terrainHeight = terrainHeight;


	PerlinNoise::initialize();
	srand(time(NULL));

	int nOfRooms = RandomFloat(140,230);
	//creating vornoi Regions with parameters
	VoronoiRegions(nOfRooms,nOfRooms/10);

	//passing the rooms to the terrain class
	rooms = m_rooms;

	//passing the corridors to the terrain class
	corridors = m_corridors;

	//releasing heightmap pointer before returning
	m_heightMap = 0;

	PerlinNoise::Release();
	return;

}

void Vornoi::VoronoiRegions(int numOfPoints = 200, int numOfRooms = 20) {
	ReleaseVornoi();

	m_VPoints = new vector<VoronoiPoint*>;
	m_VRegions = new vector<VoronoiRegion*>;

	bool resetMesh = false;
	bool randomizePoints = false;
	bool showFullDiagram = false;


	//create a grid of x*x points on the height map to be the seeds of the voronoi region. 
	if (!randomizePoints) {
		int k = 0;
		int pointsInCols = (int)sqrt((float)numOfPoints);
		int inBetweens = m_terrainWidth / pointsInCols;

		for (int j = 0; j < pointsInCols; j++) {
			for (int i = 0; i < pointsInCols; i++) {
				int nj = (inBetweens / 2);
				int index = ((j*inBetweens*m_terrainWidth)) + ((i*inBetweens)) + nj + (nj*m_terrainWidth);	//offsetting the center of the grid so that nothing is on the edges;
				index += (int)RandomFloat(-nj, nj);					//randomizing the locations
				index += (int)RandomFloat(-nj, nj)*m_terrainWidth;	//randomizing the locations

				AddVoronoiPointAt(index, k);
				k++;
			}
		}

		numOfPoints = k;
	}
	else {					//randomizing the points instead of creating a grid. (Only for testing)
		for (int k = 0; k < numOfPoints; k++) {
			int i = 0, j = 0, index = 0;

			j = (int)RandomFloat(0, m_terrainHeight - 1);
			i = (int)RandomFloat(0, m_terrainWidth - 1);
			index = (j*m_terrainWidth) + i;

			AddVoronoiPointAt(index, k);
		}
	}

	//get Voronoi data for each index of the height map
	for (int j = 0; j < m_terrainHeight; j++) {
		for (int i = 0; i < m_terrainWidth; i++)
		{
			int minIndex;
			int index = (j*m_terrainWidth) + i;
			float minDist = 100000.0f;
			for (int k = 0; k < numOfPoints; k++) {
				float dist = sqrt(pow(m_VPoints->at(k)->x - m_heightMap[index].x, 2) + pow(m_VPoints->at(k)->z - m_heightMap[index].z, 2));
				if (dist < minDist)
				{
					minDist = dist;
					minIndex = k;
				}
			}
			VoronoiData *vd = new VoronoiData;
			vd->VorPoint = m_VPoints->at(minIndex);
			vd->dist = minDist;
			m_heightMap[index].VorData = vd;
			vd = 0;

			//adding point to the region
			int RegionIndex = m_VPoints->at(minIndex)->RegionIndex;
			/*if (!m_VRegions->at(RegionIndex)->VRegionIndices) {
			m_VRegions->at(RegionIndex)->VRegionIndices = new vector<int>;
			}*/
			m_VRegions->at(RegionIndex)->VRegionIndices.push_back(index);
			//checking if the current distance is greater than the max dist (minDist is the current distance from the point)
			if (minDist > m_VRegions->at(RegionIndex)->maxDist) {
				m_VRegions->at(RegionIndex)->maxDist = minDist;
			}

		}
	}

	//if the mesh needs to be reset (Clearing all voronoi regions already present on the map)
	if (resetMesh) 
	{
		for (int j = 0; j < m_terrainHeight; j++) {
			for (int i = 0; i < m_terrainWidth; i++) {
				int index = (j*m_terrainWidth) + i;
				m_heightMap[index].y = 0.0f;
				m_heightMap[index].walkable = 0.0f;
			}
		}
	}


	//Get n unique rooms from the generated voronoi regions
	int *n = new int[numOfRooms];
	int numOfRows = sqrt(numOfPoints);
	for (int i = 0; i < numOfRooms; i++) {
		n[i] = (int)RandomFloat(numOfRows + 1, (float)numOfPoints - numOfRows - 1);
		
		if ((n[i] % numOfRows == 0) || (n[i] % numOfRows == numOfRows - 1)) //Left Right Border
		{
			i--;
			continue;
		}
		for (int j = i - 1; j >= 0; j--)
		{
			//check for adjescent room
			if (n[i] == n[j] || n[i] + 1 == n[j] || n[i] - 1 == n[j] || n[i] + numOfRows == n[j] || n[i] - numOfRows == n[j] || n[i] - numOfRows + 1 == n[j] || n[i] - numOfRows - 1 == n[j] || n[i] + 1 + numOfRows == n[j] || n[i] - 1 + numOfRows == n[j])
			{
				i--;
				break;
			}
		}
	}

	//pushing all the unique rooms into a new vector.
	for (int planes = 0; planes < numOfRooms; planes++) {
		m_rooms.push_back(m_VRegions->at(n[planes]));
	}

	//settingHeight so that the dungeon is indented / show all the voronoi regions if needed
	if (showFullDiagram) {
		for (int j = 0; j < m_terrainHeight; j++) {
			for (int i = 0; i < m_terrainWidth; i++) {

				int index = (j*m_terrainWidth) + i;
				int regionIndex = m_heightMap[index].VorData->VorPoint->RegionIndex;

				m_heightMap[index].y = 0;
				m_heightMap[index].y = m_heightMap[index].VorData->VorPoint->RegionIndex;
			}
		}
	}
	else {
		for (int planes = 0; planes < numOfRooms; planes++) {
			int s = m_VRegions->at(n[planes])->VRegionIndices.size();
			for (int i = 0; i < s; i++) {
				int index = m_VRegions->at(n[planes])->VRegionIndices.at(i);
				if (m_heightMap[index].walkable == 0.0f) {
					m_heightMap[index].y -= 5.0f;
					m_heightMap[index].walkable = GetWalkableValue(m_heightMap[index].x, m_heightMap[index].z);
				}
			}
		}
	}
	//ReleaseVornoi();
	DelanuayTriangles();
	return;

}

void Vornoi::DelanuayTriangles() {
	//If VoronoiRegions exists
	if (m_VRegions) {

#pragma region UsingExternalLibraryToCreateDelaunayTriangles

		//adding points to algorithm
		vector<Vec2f> points;
		int nPoints = m_rooms.size();
		for (int i = 0; i < nPoints; i++) {
			points.push_back(Vec2f(m_rooms.at(i)->vPoint->x, m_rooms.at(i)->vPoint->z, i));
		}
		//using algorithm to get delaunay triangluation
		Delaunay triangulation;
		vector<Triangle> triangles = triangulation.triangulate(points);
		vector<Edge> edges = triangulation.getEdges();

#pragma endregion

		//obtaining weights of edges as distances between points
		int nEdges = edges.size();
		for (std::vector< Edge >::iterator e = edges.begin(); e != edges.end(); ++e) {
			e->weight = sqrt(pow(e->p2.x - e->p1.x, 2) + pow(e->p2.y - e->p1.y, 2));
		}


		//sorting the edges array according to weights
		std::sort(edges.begin(), edges.end());

		//finding minimum spanning tree from graph obtaned in delaunay using Kruskal's Algorithm
		vector<Edge*> minSpanTree;
		for (std::vector< Edge >::iterator e = edges.begin(); e != edges.end(); ++e) 
		{
			bool duplicate = false;
			//check for duplicates
			for (std::vector< Edge* >::iterator m = minSpanTree.begin(); m != minSpanTree.end(); ++m) 
			{
				if ((*m)->p1 == (e->p1) && (*m)->p2 == (e->p2) || (*m)->p1 == (e->p2) && (*m)->p2 == (e->p1))
				{
					duplicate = true;
					break;
				}
			}

			//if its not a duplicate
			if (!duplicate) {
				minSpanTree.push_back(e._Ptr);

				if (isCircular(minSpanTree)) {
					minSpanTree.pop_back();
					//m_heightMap[0].y -= 1.0f;
				}

				if (minSpanTree.size() == nPoints - 1)
					break;		//enough points obtained
			}

		}



#pragma region AdjesencyMatrix_Sucks
		/*

		//Adding the edges to the Adgecency matrix
		Edge*** A = new Edge**[nPoints];
		for (int i = 0; i < nPoints; i++) {
		A[i] = new Edge*[nPoints];
		for (int j = i; j < nPoints; j++) {
		A[i][j] = 0;
		for (std::vector< Edge >::iterator e = edges.begin(); e != edges.end(); ++e) {
		if ((e->p1.index == i && e->p2.index == j) || (e->p1.index == j&&e->p2.index == i))
		{
		A[i][j] = e._Ptr;
		break;
		}
		}
		}
		}

		vector<Edge*> processing;
		//Using Adgesency matrix to find the minimum spanning tree
		for (int i = 0; i < nPoints; i++) {
		for (int j = i; j < nPoints; j++) {
		if (A[i][j]) {
		processing.push_back(A[i][j]);
		}
		}
		Edge* minptr = processing.at(0);
		int loc = 0;
		for (int k = 0; k < processing.size(); k++) {
		if (!processing.at(k)->used && processing.at(k) < minptr) {
		minptr = processing.at(k);
		loc = k;
		}
		}
		minptr->used = true;
		minSpanTree.push_back(minptr);
		processing.erase(processing.begin() + loc);

		if (minSpanTree.size() == nPoints - 1) {
		minptr = 0;
		break;
		}
		}

		processing.clear();
		*/
#pragma endregion

		makeCorridors(minSpanTree);

		//release Data
		points.clear();


		minSpanTree.clear();

		triangles.clear();

		edges.clear();



	}
	return;

}

bool Vornoi::isCircular(vector<Edge*> &edges) {
	int nPoints = m_rooms.size();

#pragma region creatingAdjList
	//Create Adjesency list
	vector<int> **adj = new vector<int>*[nPoints];
	for (int i = 0; i < nPoints; i++) {
		adj[i] = new vector<int>;
	}

	for (std::vector< Edge* >::iterator e = edges.begin(); e != edges.end(); ++e) {
		for (int i = 0; i < nPoints; i++) {
			if ((*e)->p1.index == i) {
				if (std::find(adj[i]->begin(), adj[i]->end(), (*e)->p2.index) == adj[i]->end())	//element p2 not found in adj list
					adj[i]->push_back((*e)->p2.index);
			}
			else if ((*e)->p2.index == i) {
				if (std::find(adj[i]->begin(), adj[i]->end(), (*e)->p1.index) == adj[i]->end())	//element p1 not found in adj list
					adj[i]->push_back((*e)->p1.index);
			}
		}
	}

#pragma endregion

	bool *visited = new bool[nPoints];

	for (int i = 0; i < nPoints; i++)
		visited[i] = false;

	//Here the back edge is used to check if the currently added edge creates a circularity in the graph.
	if (isCircular((edges.back())->p1.index, visited, adj, -1)) {
		return true;
	}

	delete[] visited;
	visited = 0;


	//releasing memory before leaving
	for (int i = 0; i < nPoints; i++) {
		adj[i]->clear();

	}
	delete[] adj;
	adj = 0;

	//returning false if no circularity
	return false;
}

bool Vornoi::isCircular(int v, bool visited[], vector<int, allocator<int>> **adj, int parent) {

	visited[v] = true;

	for (vector<int>::iterator i = adj[v]->begin(); i != adj[v]->end(); ++i) {
		int ind = (*i);
		if (!visited[ind])
		{
			if (isCircular(ind, visited, adj, v))
				return true;
		}
		else	if (ind != parent)
			return true;
	}



	return false;

}


void Vornoi::makeCorridors(const vector<Edge*> &tree) {

	for (int i = 0; i < tree.size(); i++) {
		//getting index of the point and accessing the index in terms for the heightmap
		int p1Index = m_rooms.at(tree.at(i)->p1.index)->vPoint->index;
		int p2Index = m_rooms.at(tree.at(i)->p2.index)->vPoint->index;

		//obtaining x1 nad y1 from the given index
		int x1 = p1Index % (m_terrainHeight), x2 = p2Index % (m_terrainHeight);
		int y1 = p1Index / m_terrainHeight, y2 = p2Index / m_terrainHeight;
		int ycol1 = y1, ycol2 = y2, xcol1 = x1, xcol2 = x2;
		bool yswap = false, xswap = false;
		//checking which one is smaller
		if (x1 > x2) {
			int temp = x1;
			x1 = x2;
			x2 = temp;
			/*xcol1 = x2;
			xcol2 = x1;
			*/
			xswap = true;
		}

		if (y1 > y2) {
			int temp = y1;
			y1 = y2;
			y2 = temp;
			/*ycol1 = y2;
			ycol2 = y1;
			*/
			yswap = true;
		}
		/*if (xswap && !yswap) {
		ycol1 = y2;
		ycol2 = y1;
		}
		if (yswap && !xswap) {
		xcol1 = x2;
		xcol2 = x1;
		}*/


#pragma region DrawingLines
		/*
		float m = ((float)(ycol2 - ycol1)) / ((float)(xcol2 - xcol1));
		float b = (float)ycol1 - (((float)xcol1)*m);
		for (int j = y1; j <= y2; j++)
		for (int k = x1; k <= x2; k++) {
		bool eq = ((float)j >= ((float)k*m) + b) && ((float)j <= ((float)k*m) + b + 3);
		if (eq) {
		if (m_heightMap[j*m_terrainHeight + k].walkable == 0.0f) {
		m_heightMap[j*m_terrainHeight + k].walkable = 1.0f;
		m_heightMap[j*m_terrainHeight + k].y -= 5.0f;
		}
		}
		}*/
#pragma endregion

		vector<HeightMapType*> c1, c2;
		//creating columns
		for (int j = x1; j <= x2; j++)
		{
			//giving it a height using the original position of y2
			for (int k = ycol2 - 2; k < ycol2 + 2; k++) {
				if (m_heightMap[(k*m_terrainHeight) + j].walkable == 0.0f) {
					int index = (k*m_terrainHeight) + j;
					m_heightMap[index].walkable = GetWalkableValue(m_heightMap[index].x, m_heightMap[index].z);
					m_heightMap[index].y -= 5;
				}
			}
			//adding the required points to a vector to access later
			c1.push_back(&m_heightMap[(ycol2*m_terrainHeight) + j]);
		}
		for (int j = y1; j <= y2; j++)
		{
			//giving it a width using the original position of x1
			for (int k = xcol1 - 2; k < xcol1 + 2; k++) {
				if (m_heightMap[(j*m_terrainHeight) + k].walkable == 0.0f) {
					int index = (j*m_terrainHeight) + k;
					m_heightMap[index].walkable = GetWalkableValue(m_heightMap[index].x, m_heightMap[index].z);
					m_heightMap[index].y -= 5;
				}
			}
			//adding required points to vector to access later
			c2.push_back(&m_heightMap[(j*m_terrainHeight) + xcol1]);
		}

		//collecting both the corridors into a main vector
		m_corridors.push_back(c1);
		m_corridors.push_back(c2);

	}

	return;
}


void Vornoi::ReleaseVornoi()
{
	if (m_VPoints) {
		for (std::vector< VoronoiPoint* >::iterator it = m_VPoints->begin(); it != m_VPoints->end(); ++it)
		{
			delete (*it);
		}
		m_VPoints->clear();
		delete m_VPoints;
		m_VPoints = 0;
	}

	if (m_VRegions) {
		/*for (int i = 0; i < m_VRegions->size(); i++)
		{
		for (std::vector< HeightMapType* >::iterator it = m_VRegions->at(i)->VRegions->begin(); it != m_VRegions->at(i)->VRegions->end(); ++it)
		{
		delete (*it);
		}
		m_VRegions->at(i)->VRegions->clear();
		delete m_VRegions->at(i)->VRegions;
		m_VRegions->at(i)->VRegions = 0;

		}*/
		for (std::vector< VoronoiRegion* >::iterator it = m_VRegions->begin(); it != m_VRegions->end(); ++it)
		{
			delete (*it);
		}
		m_VRegions->clear();
		delete m_VRegions;
		m_VRegions = 0;
	}

	if (!m_rooms.empty())		m_rooms.clear();
	
	if (!m_corridors.empty()) {
		for (vector<vector<HeightMapType*>>::iterator i = m_corridors.begin(); i != m_corridors.end(); ++i) {
			(*i).clear();
		}
		m_corridors.clear();
	}
	


	return;
}


float Vornoi::GetWalkableValue(float i, float j)
{
	float f = PerlinNoise::noise((double)j / 10, 1.0f, (double)i / 10);
	f = max(f*1.25f, 0.0f);				//clamping values with 0
	return f+1.0f;		//adding 1 so that player can always walk
}

float Vornoi::RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}