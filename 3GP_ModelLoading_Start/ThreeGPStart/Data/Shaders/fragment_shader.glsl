#version 330

//uniform vec4 diffuse_colour;
uniform sampler2D sampler_tex;
uniform vec3 viewPos;
uniform bool lightFlag;

in vec2 varying_texCoord;
in vec3 varying_normal;
in vec3 varying_position;	//fragPos

out vec4 fragment_colour;

struct DirLight
{
	vec3 direction;

	float ambient;
	float specular;
};
uniform DirLight dirLight;

struct PointLight
{
	vec3 position;

	float range;

	float ambient;
	float specular;
};
uniform PointLight pointLight;

struct SpotLight
{
	vec3 position;
	vec3 direction;

	float cutOff;
	float outerCutOff;
	float range;

	float ambient;
	float specular;
};
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 tex_colour);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 tex_colour);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 tex_colour);

void main(void)
{
	vec3 tex_colour = texture(sampler_tex, varying_texCoord).rgb;

	if (lightFlag == true)
	{
		//properties
		vec3 P = varying_position;	//fragPos
		vec3 N = normalize(varying_normal);
		vec3 L = normalize(viewPos - P);	//viewDir

		vec3 result = CalcDirLight(dirLight, N, L, tex_colour);
		result += CalcPointLight(pointLight, N, P, L, tex_colour);
		result += CalcSpotLight(spotLight, N, P, L, tex_colour);

		fragment_colour = vec4(result, 1.0);
	}
	else
	{
		fragment_colour = vec4(tex_colour, 1.0);
	}	
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 tex_colour)
{
	vec3 lightColour = vec3(1.0, 0.7, 0.6);
	vec3 lightDir = normalize(-light.direction);
	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	//combine results
	vec3 ambient = light.ambient * lightColour;
	vec3 diffuse = diff * lightColour;
	vec3 specular = light.specular * spec * lightColour;
	return ((ambient + diffuse + specular)* tex_colour);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 tex_colour)
{
	vec3 lightColour = vec3(1.0, 1.0, 1.0);
	vec3 lightDir = normalize(light.position - fragPos);
	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	//attenuation
	float distance = distance(light.position, fragPos);
	float attenuation = 1.0 - smoothstep(0, light.range, distance);
	//combine results
	vec3 ambient = light.ambient * lightColour;
	vec3 diffuse = diff * lightColour;
	vec3 specular = light.specular * spec * lightColour;
	//ambient *= attenuation;
	diffuse *= attenuation;
	//specular *= attenuation;

	return ((ambient + diffuse + specular) * tex_colour);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 tex_colour)
{
	vec3 lightColour = vec3(1.0, 1.0, 1.0);
	vec3 lightDir = normalize(light.position - fragPos);
	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	//attenuation
	float distance = distance(light.position, fragPos);
	float attenuation = 1.0 - smoothstep(0, light.range, distance);
	//combine results
	vec3 ambient = light.ambient * lightColour;
	vec3 diffuse = diff * lightColour;
	vec3 specular = light.specular * spec * lightColour;
	//ambient *= attenuation;
	//diffuse *= attenuation;
	//specular *= attenuation;

	//calculate soft edges
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	diffuse *= intensity;
	specular *= intensity;

	return ((ambient + diffuse + specular) * tex_colour);
}

////ambient
//vec3 lightColour = vec3(1.0, 0.7, 0.6);
////vec3 light_direction = vec3(-0.5, -0.1, -0.9);	//for directional light(sun)
////for point lights (lightbulb)		
//vec3 light_direction = P - lightPos;
//float ambientStrength = 0.1;	//pass as uniform
//vec3 ambient = ambientStrength * lightColour;
//
////normalisation
//vec3 N = normalize(varying_normal);
//vec3 L = normalize(lightPos - P); //for point light
////vec3 L = normalize(-light_direction); //for directional light
//
////specular
//float specularStrength = 0.5;	//pass as uniform
//vec3 viewDir = normalize(viewPos - varying_position);
//vec3 reflectDir = reflect(-L, N);
//
//float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//vec3 specular = specularStrength * spec * lightColour;
//
////attenuation
//float light_range = 0.9f; //pass as uniform
//float light_distance = distance(lightPos, varying_position);
//float attenuation = 1.0 - smoothstep(0.0, light_range, light_distance);
//
////diffuse
//float diff = max(0, dot(L, N));
//vec3 diffuse = diff * lightColour;
////diffuse = diffuse * attenuation;
//
//vec3 result = (ambient + diffuse + specular) * tex_colour;