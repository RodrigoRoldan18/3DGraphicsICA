#pragma once

#include "ExternalLibraryHeaders.h"

#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"

class Terrain;
class Skybox;

struct myMesh
{
	GLuint VAO;
	GLuint texture;
	Helpers::Mesh mesh;
	unsigned int numElements;
};

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

	std::vector<myMesh*> meshVector;
	myMesh* skybox;
	Skybox* mySkybox;

public:
	Renderer()=default;
	~Renderer();

	// Create and / or load geometry, this is like 'level load'
	bool InitialiseGeometry();

	// Render the scene
	void Render(const Helpers::Camera& camera, float deltaTime);

	void GenBuffers(const Helpers::Mesh& mesh, const std::vector<std::string>& texFileName);
	void GenSkyboxBuff(const Helpers::Mesh& mesh);
	GLuint loadCubemap(const std::vector<std::string>& faces);
};

