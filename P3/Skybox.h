#pragma once
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

class Skybox
{
public:
	Skybox();
	~Skybox();

	void createShader();
	void initialize();
	GLuint loadCubemap(std::vector<std::string> faces);
	
	void Draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, GLuint VAO);

private:
	GLuint skyboxRenderingProgram;
	GLuint skyboxTexture;
	
	std::string basepath = "assets/";
};

