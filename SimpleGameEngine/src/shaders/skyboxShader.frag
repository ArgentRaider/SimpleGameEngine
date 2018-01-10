# version 330 core
out vec4 FragColor;

in vec3 TexCorrds;

uniform samplerCube skybox;

void main(){
	FragColor = texture(skybox, TexCorrds);
}
