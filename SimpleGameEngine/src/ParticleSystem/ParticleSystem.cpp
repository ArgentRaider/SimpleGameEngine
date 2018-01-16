#include "ParticleSystem.h"
#include <stb_image.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

ParticleSystem::ParticleSystem()
{
	srand((unsigned int)(time(NULL)));
}

void ParticleSystem::init(const glm::vec3& Pos)
{
	Particles = std::vector<Particle>();
	initRandomVec3(10000);
	// generate initial particles
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		Particle tmp;
		// 0.03-0.07
		tmp.Size = (rand() % 5 + 3)*0.01f;
		// +/-1
		tmp.Pos = Pos;
		tmp.Pos += glm::vec3(rand()*0.25f / RAND_MAX - 0.125f, rand()*0.25f / RAND_MAX - 0.125f, rand()*0.25f / RAND_MAX - 0.125f);
		// all directions
		tmp.Vel = randomVec3[rand() % 10000] * 2.0f;
		Particles.push_back(tmp);
	}
	
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// pre-allocate buffer area
	glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * MAX_PARTICLES, NULL, GL_STATIC_DRAW);
	glBindVertexArray(0);
	
	maxLifeTime = 800.0f;
	shader.init();

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	int width, height, nrChannels;
	unsigned char* data = stbi_load("res/textures/boom.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void ParticleSystem::Render(const glm::mat4& VP, const glm::vec3& CameraPos)
{
	UpdateParticles();
	RenderParticles(VP, CameraPos);
}

void ParticleSystem::RenderParticles(const glm::mat4& VP, const glm::vec3& CameraPos)
{
	shader.use();
	shader.setMat4("gVP", VP);
	shader.setVec3("gCameraPos", CameraPos);

	if (Particles.size() > 0)
	{
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// Size
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
		// Position
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);
		// Color
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)16);
		// refresh the buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, Particles.size() * sizeof(Particle), &Particles[0]);
		// draw
		glDrawArrays(GL_POINTS, 0, Particles.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindVertexArray(0);
	}
}

void ParticleSystem::UpdateParticles()
{
	
	// delta time between each frame
	float DeltaTimeMillis = 17.0f;
	for (int i = 0; i < Particles.size(); i++)
	{
		Particles[i].Age += DeltaTimeMillis;
		if (Particles[i].Age >= maxLifeTime)
		{
			std::vector<Particle>::iterator it = Particles.begin() + i;
			Particles.erase(it);
			i--;
		}
		else
		{
			float DeltaTimeSecs = DeltaTimeMillis / 1000.0f;
			// a=-const * size^2 * v
			float coe = -6000.0f*Particles[i].Size*Particles[i].Size;
			glm::vec3 a = { coe*Particles[i].Vel.x, coe*Particles[i].Vel.y, coe*Particles[i].Vel.z };
			glm::vec3 DeltaP = DeltaTimeSecs * Particles[i].Vel;
			glm::vec3 DeltaV = DeltaTimeSecs * a;
			Particles[i].Pos += DeltaP;
			Particles[i].Vel += DeltaV;
			// stop
			if (abs(Particles[i].Vel.x) <= 0.0f || abs(Particles[i].Vel.z) <= 0.0f)
				Particles[i].Vel = { 0.0f, 0.0f, 0.0f };
			float normalAge = Particles[i].Age / 500.0f;
			Particles[i].Color = { 1.0f, 1.0f - normalAge / pow(Particles[i].Size / 0.03,5)*55.3f, 1.0f - pow(Particles[i].Age / 10.0f, 3) / 1000.0f, 1.0f - normalAge*normalAge / pow(Particles[i].Size*100.0f,5)*pow(7.0f,5) };
			if (Particles[i].Color.y <= 0.0f)
				Particles[i].Color.y = 0.0f;
			if (Particles[i].Color.z <= 0.0f)
				Particles[i].Color.z = 0.0f;
			if (Particles[i].Color.w <= 0.0f)
				Particles[i].Color.w = 0.0f;		
		}
	}
}

void ParticleSystem::initRandomVec3(unsigned int size)
{
	randomVec3 = new glm::vec3[size];
	// generate random vec3 (0.0f<=x,y,z<=1.0f)
	for (unsigned int i = 0; i < size; i++) {
		randomVec3[i].x = rand()*1.0 / RAND_MAX;
		randomVec3[i].y = rand()*1.0 / RAND_MAX;
		randomVec3[i].z = rand()*1.0 / RAND_MAX;
		randomVec3[i]-=glm::vec3(0.5f,0.5f,0.5f);
		randomVec3[i] = glm::normalize(randomVec3[i]);
	}
}