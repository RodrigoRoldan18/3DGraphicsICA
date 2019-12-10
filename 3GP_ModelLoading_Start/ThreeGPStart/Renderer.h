#pragma once

#include "ExternalLibraryHeaders.h"

#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"

class Model;
class Skybox;
class Terrain;

class Renderer
{
private:
	// Program object - to host shaders
	GLuint m_program{ 0 };

	// Vertex Array Object to wrap all render settings
//	GLuint m_VAO{ 0 };

	// Number of elments to use when rendering
//	GLuint m_numElements{ 0 };

	bool CreateProgram();

	std::vector<Model*> modelVector;

	Skybox* tempSkybox;
	Model* tempModel;
	Terrain* myTerrain;
	Model* tempAqua;

public:
	Renderer()=default;
	~Renderer();

	// Create and / or load geometry, this is like 'level load'
	bool InitialiseGeometry();

	// Render the scene
	void Render(const Helpers::Camera& camera, float deltaTime);

	std::vector<Model*>& GetModelVector() { return modelVector; }
};

