#pragma once

#include "ExternalLibraryHeaders.h"

#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"

class Renderer
{
private:
	// Program object - to host shaders
	GLuint m_program{ 0 };

	// Vertex Array Object to wrap all render settings
	GLuint m_VAO{ 0 };

	// Number of elments to use when rendering
	GLuint m_numElements{ 0 };

	bool CreateProgram();

	//--------------------------------------------------
	std::vector<glm::vec3> terrainVertices;
	std::vector<GLuint> terrainElements;
	std::vector<glm::vec2> terrainTexCoor;
	std::vector<glm::vec3> terrainNormals;
	int numCells;
	GLuint terrainTex{ 0 };
	//--------------------------------------------------

public:
	Renderer()=default;
	~Renderer();

	// Create and / or load geometry, this is like 'level load'
	bool InitialiseGeometry();

	// Render the scene
	void Render(const Helpers::Camera& camera, float deltaTime);

	void CreateTerrainElements(const int& numVertsX);
};

