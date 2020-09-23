#include "graphics.h"
#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "SDL.h"
#include <stdio.h>
#include <stdexcept>
#include <unordered_map>

void GLAPIENTRY
MessageCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam);

void Graphics::draw() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgram);
	setUniforms();
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, mapSize.x * mapSize.y);
}

void Graphics::setUniforms() {
	glUniform2i(0, mapSize.x, mapSize.y);
	glUniform2i(1, TEXTURE_SIZE, TEXTURE_SIZE);
	glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(camera.getViewProj()));
	glUniform3fv(3, 1, glm::value_ptr(camera.getPos()));
	glUniform1f(4, mod((float)SDL_GetTicks() * 0.001f, float(TEXTURE_SIZE)));
	glUniform1fv(5, 1, &height);
}

void Graphics::generateNoiseTexture(float seed) {
	glUseProgram(noiseProgram);
	glUniform1f(0, seed);
	glActiveTexture(GL_TEXTURE0);
	glBindImageTexture(0, noiseTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glDispatchCompute(TEXTURE_SIZE / 32, TEXTURE_SIZE / 32, 1);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
}

void Graphics::init(int width, int height) {
#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
#endif

	this->resolution = glm::vec2(width, height);
	createShaderProgram();

	glUseProgram(shaderProgram);

	glGenBuffers(1, &quadVBO);
	glm::vec2 vertexData[4] = { glm::vec2(0.0), glm::vec2(1.0, 0.0), glm::vec2(0.0, 1.0), glm::vec2(1.0) };
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, 0);
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glGenTextures(1, &noiseTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTextureStorage2D(noiseTexture, 1, GL_RGBA16F, TEXTURE_SIZE, TEXTURE_SIZE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_WRAP_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_WRAP_BORDER);

	glEnable(GL_DEPTH_TEST);

	createNoiseShaderProgram();
	generateNoiseTexture(0.0);
	camera = Camera(glm::vec3(-64, 100.0, 100.0), glm::vec3(128.0, 0.0, 128.0));
}

void Graphics::createShaderProgram() {
	glDeleteProgram(shaderProgram);
	uint VS = compileShader("vs.vert", GL_VERTEX_SHADER);
	uint FS = compileShader("fs.frag", GL_FRAGMENT_SHADER);
	
	//create shader program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, VS);
	glAttachShader(shaderProgram, FS);
	glLinkProgram(shaderProgram);

	GLint linkResult = GL_FALSE;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkResult);
	if (linkResult == GL_FALSE) {
		char errorBuffer[1024];
		memset(errorBuffer, 0, 1024);
		glGetProgramInfoLog(shaderProgram, 1024, nullptr, errorBuffer);
		fprintf(stderr, "%s\n", errorBuffer);
	}

	glDetachShader(shaderProgram, VS);
	glDeleteShader(VS);

	glDetachShader(shaderProgram, FS);
	glDeleteShader(FS);
}

void Graphics::createNoiseShaderProgram() {
	glDeleteProgram(noiseProgram);
	uint CS = compileShader("noise.glsl", GL_COMPUTE_SHADER);
	noiseProgram = glCreateProgram();
	glAttachShader(noiseProgram, CS);
	glLinkProgram(noiseProgram);
	
	GLint linkResult = GL_FALSE;
	glGetProgramiv(noiseProgram, GL_LINK_STATUS, &linkResult);
	if (linkResult == GL_FALSE) {
		char errorBuffer[1024];
		memset(errorBuffer, 0, 1024);
		glGetProgramInfoLog(noiseProgram, 1024, nullptr, errorBuffer);
		fprintf(stderr, "%s\n", errorBuffer);
	}

	glDetachShader(noiseProgram, CS);
	glDeleteShader(CS);
}

uint Graphics::compileShader(const char* fileName, GLenum type) {
	FILE* shaderfile = nullptr;
	uint shader = 0;
	errno_t err = 0;
	int charsRead = 0;
	GLint compileResult;
	const int BUFFER_SIZE = 4096 * 2;
	char textBuffer[BUFFER_SIZE];

	shader = glCreateShader(type);

	err = fopen_s(&shaderfile, fileName, "r");
	if (err) {
		char errormsg[100];
		sprintf_s(errormsg, 100, "error while reading %s, errno %i\n", fileName, err);
		throw std::runtime_error(errormsg);
	}

	memset(textBuffer, 0, BUFFER_SIZE);
	charsRead = (int)fread_s(textBuffer, BUFFER_SIZE, sizeof(char), BUFFER_SIZE - 1, shaderfile);
	char* textBufferArray[1] = { textBuffer };
	glShaderSource(shader, 1, textBufferArray, &charsRead);
	glCompileShader(shader);
	compileResult = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		char errorBuffer[1024];
		memset(errorBuffer, 0, 1024);
		glGetShaderInfoLog(shader, 1024, nullptr, errorBuffer);
		throw std::runtime_error(errorBuffer);
	}

	fclose(shaderfile);
	return shader;
}

Graphics::~Graphics() {

}

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	std::unordered_map<GLenum, const char*> severityMap = { 
		{ GL_DEBUG_SEVERITY_LOW, "Low" },
		{ GL_DEBUG_SEVERITY_MEDIUM, "Medium" }, 
		{ GL_DEBUG_SEVERITY_HIGH, "High" } 
	};
	
	if (type == GL_DEBUG_TYPE_ERROR)
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type,
			severityMap[severity],
			message);	
}
