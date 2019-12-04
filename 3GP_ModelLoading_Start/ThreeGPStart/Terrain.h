#pragma once
#include "ExternalLibraryHeaders.h"
#include "Helper.h"
#include "Mesh.h"
#include "Model.h"

class Terrain : public Model
{
private:
	Helpers::Mesh* myMesh;

public:
	Terrain() {};
	~Terrain() { delete myMesh; }
	void CreateTerrain(int argNumCells, float argSizeX, float argSizeZ, std::string argHeightmap);

	Helpers::Mesh GetMesh() const { return *myMesh; }
};

