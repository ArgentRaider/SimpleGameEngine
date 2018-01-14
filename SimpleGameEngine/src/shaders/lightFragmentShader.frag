#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec3 TangentFragPos;
in vec3 TangentViewPos;
in mat3 TBN;

out vec4 FragColor;

/*
 * A fragment shader for rendering mulitiple basic lights.
 *
 * Set the uniform integers "NR_DIR_LIGHTS", "NR_SPOT_LIGHTS", "NR_POINT_LIGHTS" to define 
 *	how many lights for each kind you want (100 at most). Then you can pass some parameters to the 
 *  corresponding light in the uniform arraya "dirLights", "pointLights", "spotLights". 
 *
 * For each fragment, one diffuse texture and one specular texture are allowed.
 */

uniform bool useNormalMap;

uniform vec3 viewPos;

struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	float shininess;
};

struct Light{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
//	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float cutoff;
	float outerCutoff;
};

uniform int NR_DIR_LIGHTS;
uniform int NR_POINT_LIGHTS;
uniform int NR_SPOT_LIGHTS;

uniform Material material;

#define NR_MAX_LIGHTS 10
uniform DirLight dirLights[NR_MAX_LIGHTS];
uniform PointLight pointLights[NR_MAX_LIGHTS];
uniform SpotLight spotLights[NR_MAX_LIGHTS];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){

	vec3 lightDir = normalize(-light.direction);
	if(useNormalMap){
		lightDir = TBN *  lightDir;
	}

	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

	float diff = max( dot(normal, lightDir), 0.0 );
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	if(material.shininess < 0.9){
		spec = 0.0;
	}
	
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));

	return (ambient + diffuse + specular );
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);
	if(useNormalMap){
		lightDir = TBN *  lightDir;
	}

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance \
							 + light.quadratic * distance * distance);

	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

	float diff = max( dot(normal, lightDir), 0.0 );
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	if(material.shininess < 0.9){
		spec = 0.0;
	}
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));

	return (ambient * attenuation + diffuse * attenuation + specular * attenuation);
}

// Here I ignore the ambient light of SpotLight
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.position - fragPos);
	if(useNormalMap){
		lightDir = TBN *  lightDir;
	}

	float theta = dot(lightDir, normalize(-light.direction) );
	vec3 color;
	if(theta > light.outerCutoff){
		float epsilon = light.cutoff - light.outerCutoff;
		float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

		float distance = length(light.position - fragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance \
								 + light.quadratic * distance * distance);

		float diff = max( dot(normal, lightDir), 0.0 );
		vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));

		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		if(material.shininess < 0.9){
			spec = 0.0;
		}
		vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));

		color = (diffuse + specular) * attenuation * intensity;
	}else{
		color = vec3(0.0, 0.0, 0.0);
	}
	return color;
}

void main()
{
	vec3 norm = normalize(Normal);
	if(useNormalMap){
		norm = texture(material.texture_normal1, TexCoords).rgb;
		norm = normalize(norm * 2.0 - 1.0);
	}
	vec3 viewDir = normalize(viewPos - FragPos);
	if(useNormalMap){
		viewDir = normalize(TangentViewPos - TangentFragPos);
	}

	vec3 result = vec3(0,0,0);
	for( int i = 0; i < NR_DIR_LIGHTS; i++){
		result += CalcDirLight(dirLights[i], norm, viewDir);
	}
	for( int i = 0; i < NR_POINT_LIGHTS; i++ ){
		if(useNormalMap){
			result += CalcPointLight(pointLights[i], norm, TangentFragPos, viewDir);
		}else{
			result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
		}
	}

	for( int i = 0; i < NR_SPOT_LIGHTS; i++){
		if(useNormalMap){
			result += CalcSpotLight(spotLights[i], norm, TangentFragPos, viewDir);
		}else{
			result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);
		}
	}	

	FragColor = vec4(result,1.0);
}
