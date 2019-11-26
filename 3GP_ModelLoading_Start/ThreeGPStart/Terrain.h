#pragma once
#include "ExternalLibraryHeaders.h"
#include "Helper.h"
#include "Mesh.h"

class Terrain
{
private:
	int numCells, numCellsX, numCellsZ;
	float sizeX, sizeZ;
	Helpers::Mesh* myMesh;
	GLuint heightmap;

public:
	Terrain() {};
	~Terrain() { delete myMesh; }
	void CreateTerrain(int argNumCells, float argSizeX, float argSizeZ, std::string argHeightmap);

	Helpers::Mesh GetMesh() const { return *myMesh; }
	GLuint GetHeightmap() const { return heightmap; }
};

