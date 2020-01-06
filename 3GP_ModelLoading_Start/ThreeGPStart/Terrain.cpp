#include "Terrain.h"
#include "ImageLoader.h"

void Terrain::CreateTerrain(int argNumCells, float argSizeX, float argSizeZ, std::string argHeightmap)
{
	int numCells, numCellsX, numCellsZ;
	float sizeX, sizeZ;
	GLbyte* imageData;

	numCells = argNumCells;
	numCellsX = numCells + 1;
	numCellsZ = numCells + 1;
	sizeX = argSizeX;
	sizeZ = argSizeZ;
	myMesh = new Helpers::Mesh();
	glm::vec3 start(-(argNumCells * sizeX) / 2, 0, (argNumCells * sizeZ) / 2);
	float tiles{ 10.0f };

	///Create the vertices
	///Load the heightmap
	Helpers::ImageLoader imgLoader;
	if (!imgLoader.Load(argHeightmap))
		return;

	float vertexXtoImage = (float)imgLoader.Width() / numCellsX;
	float vertexZtoImage = (float)imgLoader.Height() / numCellsZ;
	imageData = imgLoader.GetData();
	for (int cellZ = 0; cellZ < numCellsZ; cellZ++)
	{
		for (int cellX = 0; cellX < numCellsX; cellX++)
		{
			glm::vec3 pos{ start };
			pos.x += cellX * sizeX;
			pos.z -= cellZ * sizeZ;		

			///Create the texture coordinates
			float u = (float)cellX / numCells;
			float v = (float)cellZ / numCells;

			int heightMapX = (int)(u * (imgLoader.Width() - 1));
			int heightMapY = (int)(v * (imgLoader.Height() - 1));			

			size_t offset = ((size_t)(vertexXtoImage * cellX) + (size_t)(vertexZtoImage * cellZ) * imgLoader.Width()) * 4;	
			pos.y = (BYTE)imageData[offset];

			pos.y += KenPerlin(pos.x, pos.z);

			myMesh->vertices.push_back(pos);

			u *= tiles;
			v *= tiles;
			myMesh->uvCoords.push_back(glm::vec2(u, v));
			///Create the normals
			myMesh->normals.push_back({ 0.0f, 0.0f, 0.0f });
			

		}
	}	
	///Create the terrain elements
	//-------------------------------------------
	bool doDiamondPattern = true;
	//-------------------------------------------
	for (int cellZ = 0; cellZ < numCells; cellZ++)
	{
		for (int cellX = 0; cellX < numCells; cellX++)
		{
			int startVertIndex = cellZ * numCellsX + cellX;
			if (doDiamondPattern)
			{
				myMesh->elements.push_back(startVertIndex);	//0
				myMesh->elements.push_back(startVertIndex + 1);	//1
				myMesh->elements.push_back(startVertIndex + numCellsX);	//3

				myMesh->elements.push_back(startVertIndex + 1);	//1
				myMesh->elements.push_back(startVertIndex + numCellsX + 1);	//4
				myMesh->elements.push_back(startVertIndex + numCellsX);	//3
			}
			else
			{
				myMesh->elements.push_back(startVertIndex);	//0
				myMesh->elements.push_back(startVertIndex + numCellsX + 1);//4
				myMesh->elements.push_back(startVertIndex + numCellsX);//3				

				myMesh->elements.push_back(startVertIndex);	//0
				myMesh->elements.push_back(startVertIndex + 1);	//1
				myMesh->elements.push_back(startVertIndex + numCellsX + 1); //4
			}
			doDiamondPattern = !doDiamondPattern;
		}
		if (numCells % 2 == 0)
		{
			doDiamondPattern = !doDiamondPattern;
		}
	}	
	///Create the normals
	for (GLuint i = 0; i < myMesh->elements.size(); i+=3)
	{
		glm::vec3 p1 = myMesh->vertices[myMesh->elements[i]];
		glm::vec3 p2 = myMesh->vertices[myMesh->elements[i + 1]];
		glm::vec3 p3 = myMesh->vertices[myMesh->elements[i + 2]];

		glm::vec3 edge1 = p2 - p1;
		glm::vec3 edge2 = p3 - p1;
		glm::vec3 normal = glm::cross(edge1, edge2);

		myMesh->normals[myMesh->elements[i]] += normal;
		myMesh->normals[myMesh->elements[i + 1]] += normal;
		myMesh->normals[myMesh->elements[i + 2]] += normal;
	}
	for (auto& normal : myMesh->normals)
	{
		glm::normalize(normal);
	}
}

float Terrain::Noise(int x, int y)
{
	int n = x + y * 57; // 57 is the seed – can be tweaked
	n = (n >> 13) ^ n;
	int nn = (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	return 1.0f - ((float)nn / 1073741824.0f);
}

float Terrain::CosineLerp(float a, float b, float x)
{
	float ft = x * 3.1415927f;
	float f = (1.0f - cos(ft)) * 0.5f;
	return a * (1.0f - f) + b * f;
}

float Terrain::KenPerlin(float xPos, float zPos)
{
	float s = Noise((int)xPos, (int)zPos);
	float t = Noise((int)xPos + 1, (int)zPos);
	float u = Noise((int)xPos, (int)zPos + 1);
	float v = Noise((int)xPos + 1, (int)zPos + 1);
	float c1 = CosineLerp(s, t, xPos);
	float c2 = CosineLerp(u, v, xPos);
	return CosineLerp(c1, c2, zPos);
}
