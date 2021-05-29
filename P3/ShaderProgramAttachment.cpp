#include "ShaderProgramAttachment.h"
#include <fstream>
#include <glfw/glfw3.h>
#include <iostream>


ShaderProgram* ShaderProgram::sharedInstance = nullptr;

ShaderProgram* ShaderProgram::getInstance()
{
	if (sharedInstance == nullptr)
		sharedInstance = new ShaderProgram();
	return sharedInstance;
}

std::string ShaderProgram::readShaderSource(const char* filePath)
{
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);
	std::string line = "";

	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

GLuint ShaderProgram::createShaderProgram()
{
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertShaderStr = readShaderSource("vertShader.glsl");
	std::string fragShaderStr = readShaderSource("fragShader.glsl");

	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	glCompileShader(vShader);
	glCompileShader(fShader);

	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);

	return vfProgram;
}

GLuint ShaderProgram::createShadowlessShaderProgram()
{
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertShaderStr = readShaderSource("lakeVertShader.glsl");
	std::string fragShaderStr = readShaderSource("lakeFragShader.glsl");

	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	glCompileShader(vShader);
	glCompileShader(fShader);

	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);

	return vfProgram;
}

GLuint ShaderProgram::createSkyboxShaderProgram()
{
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertShaderStr = readShaderSource("skyboxVertShader.glsl");
	std::string fragShaderStr = readShaderSource("skyboxFragShader.glsl");

	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	glCompileShader(vShader);
	glCompileShader(fShader);

	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);

	return vfProgram;
}

GLuint ShaderProgram::createScreenShaderProgram()
{

	//skybox
	GLuint framevShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint framefShader = glCreateShader(GL_FRAGMENT_SHADER);

	//skybox
	std::string framevertShaderStr = readShaderSource("framevertShader.glsl");
	std::string framefragShaderStr = readShaderSource("framefragShader.glsl");

	//skybox
	const char* framevertShaderSrc = framevertShaderStr.c_str();
	const char* framefragShaderSrc = framefragShaderStr.c_str();

	//skybox
	glShaderSource(framevShader, 1, &framevertShaderSrc, NULL);
	glShaderSource(framefShader, 1, &framefragShaderSrc, NULL);

	glCompileShader(framevShader);
	glCompileShader(framefShader);

	GLuint framevfProgram = glCreateProgram();
	glAttachShader(framevfProgram, framevShader);
	glAttachShader(framevfProgram, framefShader);
	glLinkProgram(framevfProgram);

	return framevfProgram;



	
}

ShaderProgram::ShaderProgram()
{
}
