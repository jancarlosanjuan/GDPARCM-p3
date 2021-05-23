#pragma once
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>

class GameObject
{
public:
	GameObject();
	~GameObject();

	void initialize(std::string filePath, float r, float g, float b, glm::vec3 position, float rotation, GLuint texture);

	void Draw();

private:
	std::string basepath = "assets/";
	GLuint texture;

	GLuint VBO, VAO, EBO;

	int meshVertexCount;
	int meshIndicesCount;
	glm::vec3 objectLocation;
	float objectRotation;
};

