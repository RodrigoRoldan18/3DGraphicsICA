#pragma once
#include "ExternalLibraryHeaders.h"
#include "Helper.h"
#include "Mesh.h"
//#include "Node.h"

class MyMesh;

class Model
{
protected:
	std::vector<MyMesh*> meshVector;
	//GLint textureWrappingType;
	std::vector<Helpers::Material> materialVector;
	//Node* node = new Node();

public:
	void GenBuffers(const Helpers::Mesh& mesh, const std::string& texFileName, const GLint& textureWrappingType);
	virtual void Render(const GLuint& argProgram, glm::mat4& argCombined_xform, const glm::mat4& argProjection_xform, const glm::mat4& argView_xform, GLuint& argCombined_xform_id);
};

