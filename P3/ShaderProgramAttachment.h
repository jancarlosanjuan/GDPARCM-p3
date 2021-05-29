#pragma once

#include <GL/glew.h>
#include <string>

class ShaderProgram
{
public:
	static ShaderProgram* getInstance();

	std::string readShaderSource(const char* filePath);
	GLuint createShaderProgram();
	GLuint createShadowlessShaderProgram();
	GLuint createSkyboxShaderProgram();
	GLuint createScreenShaderProgram();
private:
	ShaderProgram();
	ShaderProgram(ShaderProgram const&) {};             // copy constructor is private
	ShaderProgram& operator=(ShaderProgram const&) {};  // assignment operator is private
	static ShaderProgram* sharedInstance;
};