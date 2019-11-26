#include "Terrain.h"
#include "ImageLoader.h"

void Terrain::CreateTerrain(int argNumCells, float argSizeX, float argSizeZ, std::string argHeightmap)
{
	numCells = argNumCells;
	numCellsX = numCells + 1;
	numCellsZ = numCells + 1;
	sizeX = argSizeX;
	sizeZ = argSizeZ;
	myMesh = new Helpers::Mesh();

	///Create the vertices
	for (int cellZ = 0; cellZ < numCellsZ; cellZ++)
	{
		for (int cellX = 0; cellX < numCellsX; cellX++)
		{
			//myMesh->vertices.push_back({ cellX * sizeX, rand() % ((int)sizeX) - (int)sizeX / 2, -cellZ * sizeZ });
			myMesh->vertices.push_back({ cellX * sizeX, 0.0f, -cellZ * sizeZ });
		}
	}
	///Create the texture coordinates
	for (int cellZ = 0; cellZ < numCellsZ; cellZ++)
	{
		for (int cellX = 0; cellX < numCellsX; cellX++)
		{
			myMesh->uvCoords.push_back({ (float)cellX * sizeX / numCellsX, (float)cellZ * sizeZ / numCellsZ });
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
				myMesh->elements.push_back(startVertIndex + 1);//1
				myMesh->elements.push_back(startVertIndex + numCellsX + 1);//4
				myMesh->elements.push_back(startVertIndex);	//0

				myMesh->elements.push_back(startVertIndex + numCellsX + 1); //4
				myMesh->elements.push_back(startVertIndex + numCellsX);	//3
				myMesh->elements.push_back(startVertIndex);	//0
			}
			doDiamondPattern = !doDiamondPattern;
		}
		if (numCells % 2 == 0)
		{
			doDiamondPattern = !doDiamondPattern;
		}
	}
	///Create the normals
	myMesh->normals.resize(myMesh->vertices.size());
	for (auto& v : myMesh->normals)
	{
		v = { 0.0f, 0.0f, 0.0f };
	}
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
	///Load the heightmap
	Helpers::ImageLoader imgLoader;
	if (!imgLoader.Load(argHeightmap))
		return;

	glGenTextures(1, &heightmap);
	glBindTexture(GL_TEXTURE_2D, heightmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgLoader.Width(), imgLoader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgLoader.GetData());
	glGenerateMipmap(GL_TEXTURE_2D);

}