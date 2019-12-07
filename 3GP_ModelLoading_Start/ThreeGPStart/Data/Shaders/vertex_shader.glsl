#version 330

uniform mat4 combined_xform;
uniform mat4 model_xform;
uniform sampler2D sampler_tex;
uniform vec3 lightPos;

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texCoord;

out vec2 varying_texCoord;
out vec3 varying_normal;
out vec3 FragPos;

void main(void)
{	
	varying_texCoord = vertex_texCoord;
	varying_normal = mat3(transpose(inverse(model_xform))) * vertex_normal;
	FragPos = vec3(model_xform * vec4(vertex_position, 1.0));

	gl_Position = combined_xform * model_xform * vec4(vertex_position, 1.0);
}