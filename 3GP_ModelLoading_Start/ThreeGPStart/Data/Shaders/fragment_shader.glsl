#version 330

uniform vec4 diffuse_colour;
uniform sampler2D sampler_tex;
//uniform sampler2D heightmap_tex;

in vec2 varying_texCoord;
in vec3 varying_normal;

out vec4 fragment_colour;

void main(void)
{
	vec3 tex_coord = texture(sampler_tex, varying_texCoord).rgb;
	//vec3 heightmap = texture(heightmap_tex, varying_texCoord).rgb;

	vec3 N = normalize(varying_normal);
	fragment_colour = vec4(tex_coord * N.y, 1.0);
}