#pragma once
#include "ExternalLibraryHeaders.h"
#include "Helper.h"
#include "Mesh.h"

class Skybox
{
private:
	std::vector<std::string> faces;

public:
	Skybox();
	std::vector<std::string> GetFaces() const { return faces; }

};

