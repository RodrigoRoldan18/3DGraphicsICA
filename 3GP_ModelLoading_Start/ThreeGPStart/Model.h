#pragma once
#include "ExternalLibraryHeaders.h"

struct myMesh
{
	GLuint VAO;
	GLuint texture;
	unsigned int numElements;
};

class Model
{
private:
	std::vector<myMesh> meshVector;
};

