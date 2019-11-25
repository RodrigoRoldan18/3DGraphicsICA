#version 330

uniform vec4 diffuse_colour;
uniform sampler2D sampler_tex;

in vec2 varying_texCoord;
in vec3 varying_normal;

out vec4 fragment_colour;

void main(void)
{
	vec3 tex_coord = texture(sampler_tex, varying_texCoord).rgb;
	//fragment_colour = vec4(varying_colour * varying_normal.y,1.0);
	fragment_colour = vec4(tex_coord, 1.0);
}