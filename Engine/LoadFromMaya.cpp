#include <vector>
#include "LoadFromMaya.h"

bool LoadFromMaya::LoadFromMayaNow(char* filename, char& newName, int& vertexCounted)
{
	bool result;
	//char filename[256];
	int vertexCount, textureCount, normalCount, faceCount;
	char garbage;


	// Read in the name of the model file.

	// Read in the number of vertices, tex coords, normals, and faces so that the data structures can be initialized with the exact sizes needed.
	result = ReadFileCounts(filename, vertexCount, textureCount, normalCount, faceCount);
	if (!result)
	{
		return false;
	}

	// Display the counts to the screen for information purposes.
	cout << endl;
	cout << "Vertices: " << vertexCount << endl;
	cout << "UVs:      " << textureCount << endl;
	cout << "Normals:  " << normalCount << endl;
	cout << "Faces:    " << faceCount << endl;

	// Now read the data from the file into the data structures and then output it in our model format.
	result = LoadDataStructures(filename, vertexCount, textureCount, normalCount, faceCount, newName);
	if (!result)
	{
		return false;
	}
	vertexCounted = 0;//m_numberOfFaces;
	return true;
}



LoadFromMaya::LoadFromMaya()
{
}

LoadFromMaya::~LoadFromMaya()
{
}

LoadFromMaya::LoadFromMaya(const LoadFromMaya &)
{
}

bool LoadFromMaya::ReadFileCounts(char* filename, int& vertexCount, int& textureCount, int& normalCount, int& faceCount)
{
	ifstream fin;
	char input;


	// Initialize the counts.
	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;

	// Open the file.
	fin.open(filename);

	// Check if it was successful in opening the file.
	if (fin.fail() == true)
	{
		return false;
	}

	// Read from the file and continue to read until the end of the file is reached.
	fin.get(input);
	while (!fin.eof())
	{
		// If the line starts with 'v' then count either the vertex, the texture coordinates, or the normal vector.
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ') { vertexCount++; }
			if (input == 't') { textureCount++; }
			if (input == 'n') { normalCount++; }
		}

		// If the line starts with 'f' then increment the face count.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ') { faceCount++; }
		}

		// Otherwise read in the remainder of the line.
		while (input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	return true;
}


bool LoadFromMaya::LoadDataStructures(char* filename, int vertexCount, int textureCount, int normalCount, int faceCount, char& newName)
{
	VertexType *vertices, *texcoords, *normals;
	FaceType *faces;
	ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	ofstream fout;
	ofstream fout2;
	vector<FaceType> *facesList = new vector<FaceType>;

	// Initialize the four data structures.
	vertices = new VertexType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	texcoords = new VertexType[textureCount];
	if (!texcoords)
	{
		return false;
	}

	normals = new VertexType[normalCount];
	if (!normals)
	{
		return false;
	}

	faces = new FaceType[faceCount];
	if (!faces)
	{
		return false;
	}

	// Initialize the indexes.
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;
	fout2.open("../Engine/data/ModelErrors.txt");

	// Open the file.
	fin.open(filename);

	// Check if it was successful in opening the file.
	if (fin.fail() == true)
	{
		return false;
	}

	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			// Read in the vertices.
			if (input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				// Invert the Z vertex to change to left hand system.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

			// Read in the texture uv coordinates.
			if (input == 't')
			{
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				// Invert the V texture coordinates to left hand system.
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++;
			}

			// Read in the normals.
			if (input == 'n')
			{
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				// Invert the Z normal to change to left hand system.
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++;
			}
		}

		// Read in the faces.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				// Read the face data in backwards to convert it to a left hand system from right hand system.
				vector<FaceType> *temp = new vector<FaceType>;
				while (input != '\n') {
					FaceType f;
					fin >> f.vIndex >> input2 >> f.tIndex >> input2 >> f.nIndex;

					temp->insert(temp->begin(), f);
					facesList->push_back(f);
					fin.get(input);
				}

				{
					int size = temp->size();
					switch (size) {
					case 3: {
						facesList->push_back(temp->at(0));
						facesList->push_back(temp->at(1));
						facesList->push_back(temp->at(2));
						fout2 << "m";
						break;
					}
					case 4: {
						facesList->push_back(temp->at(0));
						facesList->push_back(temp->at(1));
						facesList->push_back(temp->at(2));
						facesList->push_back(temp->at(0));
						facesList->push_back(temp->at(2));
						facesList->push_back(temp->at(3));
						break;
					}
					default: {
						break;
					}
					}
				}
				temp->clear();
				faceIndex++;


			}
		}
		else {
			// Read in the remainder of the line.
			while (input != '\n')
			{
				fin.get(input);
			}
		}
		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();
	fout2.close();

	m_numberOfFaces = facesList->size();
	// Open the output file.
	fout.open("../Engine/data/newModel.txt");
	//newName = "../NamanEngine/data/newModel.txt";

	// Write out the file header that our model format uses.
	fout << "Vertex Count: " << (facesList->size()) << endl;
	fout << endl;
	fout << "Data:" << endl;
	fout << endl;

	// Now loop through all the faces and output the three vertices for each face.
	for (int i = 0; i<facesList->size(); i++)
	{
		vIndex = facesList->at(i).vIndex - 1;
		tIndex = facesList->at(i).tIndex - 1;
		nIndex = facesList->at(i).nIndex - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

	}

	// Close the output file.
	fout.close();

	// Release the four data structures.
	if (vertices)
	{
		delete[] vertices;
		vertices = 0;
	}
	if (texcoords)
	{
		delete[] texcoords;
		texcoords = 0;
	}
	if (normals)
	{
		delete[] normals;
		normals = 0;
	}
	if (faces)
	{
		delete[] faces;
		faces = 0;
	}

	facesList->clear();

	return true;
}
