#include "Model.h"
#include "MyMesh.h"
#include "ImageLoader.h"

Model::~Model()
{
	for (auto& mesh : meshVector)
	{
		delete mesh;
	}
	delete root;
}

void Model::GenBuffers(const Helpers::Mesh& mesh, const std::string& texFileName, const GLint& textureWrappingType)
{
	MyMesh* temp = new MyMesh();
	GLuint positionsVBO;
	GLuint normalsVBO;
	GLuint texVBO;
	GLuint elementsEBO;
	GLuint tempTexture{ 0 };
	glGenBuffers(1, &positionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &normalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.normals.size(), mesh.normals.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &texVBO);
	glBindBuffer(GL_ARRAY_BUFFER, texVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvCoords.size(), mesh.uvCoords.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &elementsEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.elements.size(), mesh.elements.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	temp->numElements = mesh.elements.size();

	glGenVertexArrays(1, &temp->VAO);
	glBindVertexArray(temp->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ARRAY_BUFFER, texVBO);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);

	// Good idea to check for an error now:	
	Helpers::CheckForGLError();

	// Clear VAO binding
	glBindVertexArray(0);

	Helpers::ImageLoader imgLoader;
	if (!imgLoader.Load(texFileName))
		return;

	glGenTextures(1, &tempTexture);
	glBindTexture(GL_TEXTURE_2D, tempTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrappingType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrappingType);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgLoader.Width(), imgLoader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgLoader.GetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	temp->texture = tempTexture;

	meshVector.push_back(temp);
}

void Model::Render(const GLuint& argProgram, glm::mat4& argCombined_xform, const glm::mat4& argProjection_xform, const glm::mat4& argView_xform, GLuint& argCombined_xform_id)
{
	// TODO: render each mesh. Send the correct model matrix to the shader in a uniform
	glm::mat4 model_xform = glm::translate(glm::mat4(1), root->translation);
	GLuint model_xform_id = glGetUniformLocation(argProgram, "model_xform");
	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

	GLuint lightFlag_id = glGetUniformLocation(argProgram, "lightFlag");
	glUniform1i(lightFlag_id, true);
	for (MyMesh* mesh : meshVector)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->texture);
		glUniform1i(glGetUniformLocation(argProgram, "sampler_tex"), 0);

		glBindVertexArray(mesh->VAO);
		glDrawElements(GL_TRIANGLES, mesh->numElements, GL_UNSIGNED_INT, (void*)0);
		// Always a good idea, when debugging at least, to check for GL errors
		Helpers::CheckForGLError();
	}		
}

void Model::LoadMaterials(const std::vector<Helpers::Material>& argMaterialVec, const std::string& argFilePath)
{
	materialVector = argMaterialVec;
	for (Helpers::Material& mat : materialVector)
	{
		mat.diffuseTextureFilename = argFilePath + mat.diffuseTextureFilename;
		std::cout << mat.diffuseTextureFilename << std::endl;
	}
}

void Model::LoadHierarchy(Helpers::Node* argNode)
{
	//root = argNode;
}

void Model::UpdateHierarchy()
{
}
