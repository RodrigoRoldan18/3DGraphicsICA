#include "Renderer.h"
#include "ImageLoader.h"

// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	glDeleteProgram(m_program);	
	glDeleteBuffers(1, &m_VAO);
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
	numCells = 100;
	int numVertsX = numCells + 1;
	int numVertsZ = numCells + 1;
	float sizeX = 10.0f;
	float sizeZ = 10.0f;
	///Create the vertices
	for (int cellZ = 0; cellZ < numVertsZ; cellZ++)
	{
		for (int cellX = 0; cellX < numVertsX; cellX++)
		{
			terrainVertices.push_back({ cellX * sizeX, rand() % ((int)sizeX) - (int)sizeX / 2, -cellZ * sizeZ });
			//terrainVertices.push_back({ cellX * sizeX, 0.0f, -cellZ * sizeZ });
		}
	}
	///Create the texture coordinates
	for (int cellZ = 0; cellZ < numVertsZ; cellZ++)
	{
		for (int cellX = 0; cellX < numVertsX; cellX++)
		{
			terrainTexCoor.push_back({ (float)cellX * sizeX / numVertsX, (float)cellZ * sizeZ / numVertsZ });
		}
	}
	CreateTerrainElements(numVertsX);
	///Create the normals
	terrainNormals.resize(terrainVertices.size());
	for (auto& v : terrainNormals)
	{
		v = { 0.0f, 0.0f, 0.0f };
	}
	//int countNormals = 0;
	//for (int i = 0; i < terrainElements.size(); i+=3)
	//{
	//	glm::vec3 p1 = terrainVertices[terrainElements[i]];
	//	glm::vec3 p2 = terrainVertices[terrainElements[i + 1]];
	//	glm::vec3 p3 = terrainVertices[terrainElements[i + 2]];

	//	glm::vec3 edge1 = p2 - p1;
	//	glm::vec3 edge2 = p3 - p1;
	//	glm::vec3 normal = glm::cross(edge1, edge2);

	//	terrainNormals[countNormals] += normal;
	//	countNormals++;
	//}
	//for (auto& normal : terrainNormals)	//do this AFTER you have moved displaced your verts using the heightmap
	//{
	//	glm::normalize(normal);
	//}

	GLuint terrainPosVBO;
	glGenBuffers(1, &terrainPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * terrainVertices.size(), terrainVertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint terrainTexCoorVBO;
	glGenBuffers(1, &terrainTexCoorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainTexCoorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * terrainTexCoor.size(), terrainTexCoor.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint terrainNormalVBO;
	glGenBuffers(1, &terrainNormalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainNormalVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * terrainNormals.size(), terrainNormals.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint terrainIndicesEBO;
	glGenBuffers(1, &terrainIndicesEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIndicesEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * terrainElements.size(), terrainElements.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, terrainPosVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ARRAY_BUFFER, terrainTexCoorVBO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ARRAY_BUFFER, terrainNormalVBO);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIndicesEBO);

	Helpers::ImageLoader imgLoader;
	if (!imgLoader.Load("Data\\Textures\\grass11.bmp"))
		return false;

	glGenTextures(1, &terrainTex);
	glBindTexture(GL_TEXTURE_2D, terrainTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgLoader.Width(), imgLoader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgLoader.GetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	// Good idea to check for an error now:	
	Helpers::CheckForGLError();

	// Clear VAO binding
	glBindVertexArray(0);

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
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, 1.0f, 2000.0f);

	// TODO: Compute camera view matrix and combine with projection matrix for passing to shader
	glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
	glm::mat4 combined_xform = projection_xform * view_xform;

	// Use our program. Doing this enables the shaders we attached previously.
	glUseProgram(m_program);
	
	// TODO: Send the combined matrix to the shader in a uniform
	GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
	glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

	//Adding textures into GLSL	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTex);
	glUniform1i(glGetUniformLocation(m_program, "sampler_tex"), 0);

	// TODO: render each mesh. Send the correct model matrix to the shader in a uniform
	glm::mat4 model_xform = glm::mat4(1);
	GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");
	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

	// Bind our VAO and render
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, terrainElements.size(), GL_UNSIGNED_INT, (void*)0);

	// Always a good idea, when debugging at least, to check for GL errors
	Helpers::CheckForGLError();
}

void Renderer::CreateTerrainElements(const int& numVertsX)
{
	//-------------------------------------------
	bool doDiamondPattern = true;
	//-------------------------------------------
	for (int cellZ = 0; cellZ < numCells; cellZ++)
	{
		//doDiamondPattern = !doDiamondPattern;
		for (int cellX = 0; cellX < numCells; cellX++)
		{
			int startVertIndex = cellZ * numVertsX + cellX;
			if (doDiamondPattern)
			{
				terrainElements.push_back(startVertIndex);	//0
				terrainElements.push_back(startVertIndex + 1);	//1
				terrainElements.push_back(startVertIndex + numVertsX);	//3

				terrainElements.push_back(startVertIndex + 1);	//1
				terrainElements.push_back(startVertIndex + numVertsX + 1);	//4
				terrainElements.push_back(startVertIndex + numVertsX);	//3
			}
			else
			{
				terrainElements.push_back(startVertIndex + 1);//1
				terrainElements.push_back(startVertIndex + numVertsX + 1);//4
				terrainElements.push_back(startVertIndex);	//0

				terrainElements.push_back(startVertIndex + numVertsX + 1); //4
				terrainElements.push_back(startVertIndex + numVertsX);	//3
				terrainElements.push_back(startVertIndex);	//0
			}
			doDiamondPattern = !doDiamondPattern;
		}
		if (numCells % 2 == 0)
		{
			doDiamondPattern = !doDiamondPattern;
		}
	}
}
