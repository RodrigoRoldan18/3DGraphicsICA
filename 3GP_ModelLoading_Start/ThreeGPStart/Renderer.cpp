#include "Renderer.h"
#include "ImageLoader.h"
#include "Terrain.h"
#include "Skybox.h"

// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	glDeleteProgram(m_program);	
	//glDeleteBuffers(1, &m_VAO);
	/*for (auto& m : meshVector)
	{
		delete m;
	}
	for (auto& s : skyboxVector)
	{
		delete s;
	}*/
	//delete mySkybox;
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

	//mySkybox = new Skybox();
	Helpers::ModelLoader skyboxLoader;
	if (!skyboxLoader.LoadFromFile("Data\\Sky\\Clouds\\skybox.x"))
		return false;
	for (const Helpers::Mesh& mesh : skyboxLoader.GetMeshVector())
	{
		skyboxVector.push_back(GenBuffers(mesh, "Data\\Sky\\Clouds\\SkyBox_Front.tga"));
	}

	Terrain* myTerrain = new Terrain();
	myTerrain->CreateTerrain(256, 10.0f, 10.0f, "Data\\Textures\\curvy.gif");
	meshVector.push_back(GenBuffers(myTerrain->GetMesh(), "Data\\Textures\\grass11.bmp"));

	Helpers::ModelLoader jeepLoader;
	if (!jeepLoader.LoadFromFile("Data\\Models\\Jeep\\jeep.obj"))
		return false;
	for (const Helpers::Mesh& mesh : jeepLoader.GetMeshVector())
	{
		meshVector.push_back(GenBuffers(mesh, "Data\\Models\\Jeep\\jeep_rood.jpg"));
	}	
	
	/*if (!modelLoader.LoadFromFile("Data\\Models\\AquaPig\\gun.obj"))
		return false;
	for (const Helpers::Mesh& mesh : modelLoader.GetMeshVector())
	{
		GenBuffers(mesh, { "Data\\Models\\AquaPig\\aqua_pig_1K.png" });
	}
	if (!modelLoader.LoadFromFile("Data\\Models\\AquaPig\\gun_base.obj"))
		return false;
	for (const Helpers::Mesh& mesh : modelLoader.GetMeshVector())
	{
		GenBuffers(mesh, {"Data\\Models\\AquaPig\\aqua_pig_1K.png"});
	}
	if (!modelLoader.LoadFromFile("Data\\Models\\AquaPig\\hull.obj"))
		return false;
	for (const Helpers::Mesh& mesh : modelLoader.GetMeshVector())
	{
		GenBuffers(mesh, { "Data\\Models\\AquaPig\\aqua_pig_1K.png" });
	}
	if (!modelLoader.LoadFromFile("Data\\Models\\AquaPig\\propeller.obj"))
		return false;
	for (const Helpers::Mesh& mesh : modelLoader.GetMeshVector())
	{
		GenBuffers(mesh, { "Data\\Models\\AquaPig\\aqua_pig_1K.png" });
	}
	if (!modelLoader.LoadFromFile("Data\\Models\\AquaPig\\wing_left.obj"))
		return false;
	for (const Helpers::Mesh& mesh : modelLoader.GetMeshVector())
	{
		GenBuffers(mesh, { "Data\\Models\\AquaPig\\aqua_pig_1K.png" });
	}
	if (!modelLoader.LoadFromFile("Data\\Models\\AquaPig\\wing_right.obj"))
		return false;
	for (const Helpers::Mesh& mesh : modelLoader.GetMeshVector())
	{
		GenBuffers(mesh, { "Data\\Models\\AquaPig\\aqua_pig_1K.png" });
	}*/

	return true;
}

// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{		
	// Configure pipeline settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Uncomment to render in wireframe (can be useful when debugging)
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
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

	// TODO: render each mesh. Send the correct model matrix to the shader in a uniform
	glm::mat4 model_xform = glm::mat4(1);
	GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");
	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

	//Render the skybox
	for (auto& s : skyboxVector)
	{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, s.texture);
			glUniform1i(glGetUniformLocation(m_program, "sampler_tex"), 0);

			glBindVertexArray(s.VAO);
			glDrawElements(GL_TRIANGLES, s.numElements, GL_UNSIGNED_INT, (void*)0);
			// Always a good idea, when debugging at least, to check for GL errors
			Helpers::CheckForGLError();

	}	

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	combined_xform = projection_xform * view_xform;
	combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
	glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

	for (auto& m : meshVector)
	{
		// Bind our VAO and render
		
		//Adding textures into GLSL	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m.texture);
		glUniform1i(glGetUniformLocation(m_program, "sampler_tex"), 0);
		
		glBindVertexArray(m.VAO);
		glDrawElements(GL_TRIANGLES, m.numElements, GL_UNSIGNED_INT, (void*)0);
		// Always a good idea, when debugging at least, to check for GL errors
		Helpers::CheckForGLError();
	}	
}

myMesh Renderer::GenBuffers(const Helpers::Mesh& mesh, const std::string& texFileName)
{
	myMesh temp;
	GLuint positionsVBO;
	GLuint normalsVBO;
	GLuint texVBO;
	GLuint elementsEBO;
	GLuint tempTexture{0};
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

	temp.numElements = mesh.elements.size();

	glGenVertexArrays(1, &temp.VAO);
	glBindVertexArray(temp.VAO);

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
		return myMesh();

	glGenTextures(1, &tempTexture);
	glBindTexture(GL_TEXTURE_2D, tempTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgLoader.Width(), imgLoader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgLoader.GetData());
	glGenerateMipmap(GL_TEXTURE_2D);
	
	temp.texture = tempTexture;
//	temp->mesh = mesh;
	return temp;
}

GLuint Renderer::loadCubemap(const std::vector<std::string>& faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		Helpers::ImageLoader imgLoader;
		if (imgLoader.Load(faces[i]))
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imgLoader.Width(), imgLoader.Height(), 0, GL_RGB, GL_UNSIGNED_BYTE, imgLoader.GetData());
			//stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			//stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
