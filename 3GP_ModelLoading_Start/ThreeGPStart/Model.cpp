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
	GLuint lightFlag_id = glGetUniformLocation(argProgram, "lightFlag");
	glUniform1i(lightFlag_id, true);

	int meshIndex = 0;
	for (MyMesh* mesh : meshVector)
	{
		// TODO: render each mesh. Send the correct model matrix to the shader in a uniform
		glm::mat4 model_xform = glm::mat4(1);
		glm::mat4 translate_xform = glm::mat4(1);
		glm::mat4 rotate_xform = glm::mat4(1);
		if(meshIndex == 0) { translate_xform = glm::translate(translate_xform, root->translation); }
		else if (meshIndex == 5) { translate_xform = glm::translate(translate_xform, root->children.back()->children.front()->translation + root->children.back()->translation); }
		else
		{
			for (Node* node : root->children)
			{
				if (node->meshIndex == meshIndex)
				{
					translate_xform = glm::translate(translate_xform, node->translation);
					if (node->rotation.x > 0) { rotate_xform = glm::rotate(rotate_xform, node->rotation.x, node->rotation); }
					if (node->rotation.y > 0) { rotate_xform = glm::rotate(rotate_xform, node->rotation.y, node->rotation); }
					if (node->rotation.z > 0) { rotate_xform = glm::rotate(rotate_xform, node->rotation.z, node->rotation); }
					break;
				}
			}
		}
		model_xform = translate_xform * rotate_xform;
		
		GLuint model_xform_id = glGetUniformLocation(argProgram, "model_xform");
		glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->texture);
		glUniform1i(glGetUniformLocation(argProgram, "sampler_tex"), 0);

		glBindVertexArray(mesh->VAO);
		glDrawElements(GL_TRIANGLES, mesh->numElements, GL_UNSIGNED_INT, (void*)0);
		// Always a good idea, when debugging at least, to check for GL errors
		Helpers::CheckForGLError();
		meshIndex++;
	}		
}

void Model::LoadMaterials(const std::vector<Helpers::Material>& argMaterialVec, const std::string& argFilePath)
{
	materialVector = argMaterialVec;
	for (Helpers::Material& mat : materialVector)
	{
		if (mat.diffuseTextureFilename != "")
		{
			mat.diffuseTextureFilename = argFilePath + mat.diffuseTextureFilename;
			std::cout << mat.diffuseTextureFilename << std::endl;
		}
		else
		{
			mat.diffuseTextureFilename = argFilePath + "aqua_pig_2K.png";
			std::cout << mat.diffuseTextureFilename << std::endl;
		}		
	}
}

void Model::LoadHierarchy()
{
	//right_wing
	Node* rw = new Node();
	rw->translation = glm::vec3(-2.231, 0.272, -2.663);
	rw->meshIndex = 1;
	root->children.push_back(rw);
	
	//left_wing
	Node* lw = new Node();
	lw->translation = glm::vec3(2.231, 0.272, -2.663);
	lw->meshIndex = 2;
	root->children.push_back(lw);

	//propeller
	Node* p = new Node();
	p->translation = glm::vec3(0, 1.395, -3.616);
	p->rotation = glm::vec3(90, 0, 0);
	p->meshIndex = 3;
	root->children.push_back(p);

	//gun_base
	Node* gb = new Node();
	gb->translation = glm::vec3(0, 0.569, -1.866);
	gb->meshIndex = 4;
	root->children.push_back(gb);

	//gun
	Node* g = new Node();
	g->translation = glm::vec3(0, 1.506, 0.644);
	g->meshIndex = 5;
	gb->children.push_back(g);

	//delete rw;
	//delete lw;
	//delete p;
	//delete gb;
	//delete g;
}