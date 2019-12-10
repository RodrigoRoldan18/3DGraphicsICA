#version 330

//uniform vec4 diffuse_colour;
uniform sampler2D sampler_tex;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool lightFlag;

in vec2 varying_texCoord;
in vec3 varying_normal;
in vec3 FragPos;

out vec4 fragment_colour;

void main(void)
{
	vec3 tex_colour = texture(sampler_tex, varying_texCoord).rgb;

	if (lightFlag == true)
	{
		//ambient
		vec3 lightColour = vec3(1.0, 0.7, 0.6);
		float ambientStrength = 0.1;
		vec3 ambient = ambientStrength * lightColour;

		//normalisation
		vec3 N = normalize(varying_normal);
		vec3 lightDir = normalize(lightPos - FragPos);

		//specular
		float specularStrength = 0.5;
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, N);

		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = specularStrength * spec * lightColour;

		//diffuse
		float diff = max(0, dot(lightDir, N));
		vec3 diffuse = diff * lightColour;

		vec3 result = (ambient + diffuse + specular) * tex_colour;

		//tex_colour *= varying_normal.y;

		fragment_colour = vec4(result, 1.0);
	}
	else
	{
		fragment_colour = vec4(tex_colour, 1.0);
	}

	
}