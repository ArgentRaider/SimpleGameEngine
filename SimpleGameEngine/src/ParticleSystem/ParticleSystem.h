#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <ShaderProgram/Shader.h>

#define MAX_PARTICLES 1000

struct Particle {
	float Size;
	glm::vec3 Pos;
	glm::vec4 Color;
	glm::vec3 Vel;
	float Age;

	Particle(float Size, glm::vec3 Pos, glm::vec4 Color, glm::vec3 Vel, float Age) : Size(Size), Pos(Pos), Color(Color), Vel(Vel), Age(Age) {}
	Particle() {
		Size = 0.0f;
		Pos = { 0.0f, 0.0f, 0.0f };
		Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Vel = { 0.0f, 0.0f, 0.0f };
		Age = 0.0f;
	}
};

class ParticleSystem {
public:
	ParticleSystem();
	void init();
	void initParticles(const glm::vec3& Pos);
	void Render(const glm::mat4& VP, const glm::vec3& CameraPos);
private:
	void RenderParticles(const glm::mat4& VP, const glm::vec3& CameraPos);
	void UpdateParticles();
	void initRandomVec3(unsigned int size);

	unsigned int vao, vbo;
	unsigned int texture;
	BillboardShader shader;
	float maxLifeTime;
	std::vector<Particle> Particles;
	glm::vec3* randomVec3;
};

#endif