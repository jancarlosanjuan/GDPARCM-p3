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

	void initialize();
	GLuint loadCubemap(std::vector<std::string> faces);
	
	void Draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

private:
	GLuint skyboxTexture;
	
	GLuint VBO, VAO, EBO;

	int meshVertexCount;
	int meshIndicesCount;
	glm::vec3 objectLocation;
	float objectRotation;
	
	std::string basepath = "assets/";
};

