#pragma once
#include "GL/glew.h"
#include "vec2.hpp"
#include "camera.h"

typedef unsigned int uint;

class Graphics {
private:
	uint quadVAO;
	uint quadVBO;
	uint shaderProgram;
	uint noiseProgram;
	uint noiseTexture;
	glm::vec2 resolution;
	const int TEXTURE_SIZE = 256 * 8;
	glm::ivec2 mapSize = glm::ivec2(TEXTURE_SIZE);
	float height = 40.0f;

	uint compileShader(const char* fileName, GLenum shadertype);
	void setUniforms();
public:
	~Graphics();
	void init(int width, int height);
	void createShaderProgram();
	void createNoiseShaderProgram();
	void generateNoiseTexture(float seed);
	void draw();

	Camera camera;
};
