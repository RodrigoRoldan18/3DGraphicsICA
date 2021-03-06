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
	Terrain(const std::string& argName) : Model(argName) {};
	~Terrain() { delete myMesh; }
	void CreateTerrain(int argNumCells, float argSizeX, float argSizeZ, std::string argHeightmap);

	float Noise(int x, int y);
	float CosineLerp(float a, float b, float x);
	float KenPerlin(float xPos, float zPos);

	Helpers::Mesh GetMesh() const { return *myMesh; }
};

