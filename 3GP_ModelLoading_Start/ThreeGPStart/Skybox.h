#pragma once
#include "ExternalLibraryHeaders.h"
#include "Helper.h"
#include "Mesh.h"
#include "Model.h"

class Skybox : public Model
{
public:
	Skybox(const std::string& argName) : Model(argName) {};
	void Render(const GLuint& argProgram, glm::mat4& argCombined_xform, const glm::mat4& argProjection_xform, const glm::mat4& argView_xform, GLuint& argCombined_xform_id) override final;

};

