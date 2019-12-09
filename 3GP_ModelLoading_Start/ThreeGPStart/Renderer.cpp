#include "Renderer.h"
#include "ImageLoader.h"
#include "Terrain.h"
#include "Skybox.h"
#include "Model.h"
#include "Node.h"

// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	glDeleteProgram(m_program);	

	for (auto& model : modelVector)
	{
		delete model;
	}
	delete tempModel;
	delete tempSkybox;
	delete myTerrain;
	delete tempAqua;
}

// Load, compile and link the shaders and create a program object to host them
bool Renderer::CreateProgram()
{
	// Create a new program (returns a unqiue id)
	m_program = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/vertex_shader.glsl") };
	GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/fragment_shader.glsl") };
	if (vertex_shader == 0 || fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(m_program, vertex_shader);

	// The attibute 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(m_program, fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(m_program))
		return false;

	return !Helpers::CheckForGLError();
}

// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	// Load and compile shaders into m_program
	if (!CreateProgram())
		return false;

	// TODO - load mesh using the Helpers::ModelLoader class
	Helpers::ModelLoader skyboxLoader;
	if (!skyboxLoader.LoadFromFile("Data\\Sky\\Hills\\skybox.x"))
		return false;
	tempSkybox = new Skybox();
	tempSkybox->LoadMaterials(skyboxLoader.GetMaterialVector(), "Data\\Sky\\Hills\\");
	for (const Helpers::Mesh& mesh : skyboxLoader.GetMeshVector())
	{	
		tempSkybox->GenBuffers(mesh, tempSkybox->GetMaterialVec()[mesh.materialIndex].diffuseTextureFilename, GL_CLAMP_TO_EDGE);
	}
	modelVector.push_back(tempSkybox);

	myTerrain = new Terrain();
	myTerrain->CreateTerrain(256, 10.0f, 10.0f, "Data\\Textures\\curvy.gif");
	myTerrain->GenBuffers(myTerrain->GetMesh(), "Data\\Textures\\grass11.bmp", GL_REPEAT);
	modelVector.push_back(myTerrain);

	Helpers::ModelLoader jeepLoader;
	if (!jeepLoader.LoadFromFile("Data\\Models\\Jeep\\jeep.obj"))
		return false;
	tempModel = new Model();
	tempModel->LoadMaterials(jeepLoader.GetMaterialVector(), "Data\\Models\\Jeep\\");
	for (const Helpers::Mesh& mesh : jeepLoader.GetMeshVector())
	{
		tempModel->GenBuffers(mesh, tempModel->GetMaterialVec()[mesh.materialIndex].diffuseTextureFilename, GL_REPEAT);
	}	
	modelVector.push_back(tempModel);

	/*Helpers::ModelLoader aquaPigLoader;
	if (!aquaPigLoader.LoadFromFile("Data\\Models\\AquaPig\\aqua_pig_xforms.txt"))
		return false;
	tempAqua = new Model();
	tempAqua->LoadMaterials(aquaPigLoader.GetMaterialVector(), "Data\\Models\\AquaPig\\");
	tempAqua->LoadHierarchy(aquaPigLoader.GetRootNode());*/


	return true;
}

// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{		
	// Configure pipeline settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Uncomment to render in wireframe (can be useful when debugging)
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// Clear buffers from previous frame
	glClearColor(0.0f, 0.0f, 0.0f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// TODO: Compute viewport and projection matrix
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	const float aspect_ratio = viewportSize[2] / (float)viewportSize[3];
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 2000.0f);

	// TODO: Compute camera view matrix and combine with projection matrix for passing to shader
	glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
	glm::mat4 view_xform2 = glm::mat4(glm::mat3(view_xform));
	glm::mat4 combined_xform = projection_xform * view_xform2;

	// Use our program. Doing this enables the shaders we attached previously.
	glUseProgram(m_program);
	
	// TODO: Send the combined matrix to the shader in a uniform
	GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
	glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

	//Send the lightPosition to the shader in a uniform
	GLuint lightPos_id = glGetUniformLocation(m_program, "lightPos");
	glUniform3fv(lightPos_id, 1, glm::value_ptr(camera.GetPosition()));//glm::vec3(1.2f, 1.0f, 2.0f)));

	//Send the viewPosition to the shader in a uniform
	GLuint viewPos_id = glGetUniformLocation(m_program, "viewPos");
	glUniform3fv(viewPos_id, 1, glm::value_ptr(camera.GetPosition()));

	// TODO: render each mesh. Send the correct model matrix to the shader in a uniform
	glm::mat4 model_xform = glm::mat4(1);	//move this to each model as the glm::translate is applied here <--
	GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");
	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

	for (Model* m : modelVector)
	{
		m->Render(m_program, combined_xform, projection_xform, view_xform, combined_xform_id);
	}	
}