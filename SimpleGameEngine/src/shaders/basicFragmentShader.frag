#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

/*
 * Basic fragment shader. Only paint the diffuse map of the fragment.
 */

struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
};
uniform Material material;

void main()
{    
    FragColor = texture(material.texture_diffuse1, TexCoords);
}