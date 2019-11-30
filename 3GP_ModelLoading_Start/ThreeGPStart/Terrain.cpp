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
	///Load the heightmap
	Helpers::ImageLoader imgLoader;
	if (!imgLoader.Load(argHeightmap))
		return;

	vertexXtoImage = (float)imgLoader.Width() / numCellsX;
	vertexZtoImage = (float)imgLoader.Height() / numCellsZ;

	imageData = imgLoader.GetData();
	for (int cellZ = 0; cellZ < numCellsZ; cellZ++)
	{
		for (int cellX = 0; cellX < numCellsX; cellX++)
		{
			int imageX = vertexXtoImage * cellX;
			int imageZ = vertexZtoImage * cellZ;

			size_t offset = ((size_t)imageX + (size_t)imageZ * imgLoader.Width()) * 4;
			BYTE height = imageData[offset];
			myMesh->vertices.push_back({ cellX * sizeX, (float)height, -cellZ * sizeZ });
			///Create the texture coordinates
			myMesh->uvCoords.push_back({ (float)cellX * sizeX / numCellsX, (float)cellZ * sizeZ / numCellsZ });
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
