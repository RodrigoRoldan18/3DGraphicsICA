#pragma once
#include "ExternalLibraryHeaders.h"
#include "Helper.h"
#include "Mesh.h"
#include "Node.h"

class MyMesh;

class Model
{
protected:
	std::vector<MyMesh*> meshVector;
	std::string name;
	std::vector<Helpers::Material> materialVector;
	Node* root;

	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

public:
	Model(const std::string& argName) : name(argName) { root = new Node(); }
	Model(const std::string& argName, const glm::vec3& argScale) : name(argName), scale(argScale) { root = new Node(); }
	~Model();
	void GenBuffers(const Helpers::Mesh& mesh, const std::string& texFileName, const GLint& textureWrappingType);
	virtual void Render(const GLuint& argProgram, glm::mat4& argCombined_xform, const glm::mat4& argProjection_xform, const glm::mat4& argView_xform, GLuint& argCombined_xform_id);
	void LoadMaterials(const std::vector<Helpers::Material>& argMaterialVec, const std::string& argFilePath);
	void LoadHierarchy(const glm::vec3& argTranslation);
	void CreateLightbulb();

	const std::vector<Helpers::Material>& GetMaterialVec() const { return materialVector; }
	const std::string& GetName() const { return name; }
	Node* GetRoot() const { return root; }
};

