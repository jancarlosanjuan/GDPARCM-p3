#pragma once
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

class GameObject
{
public:
	GameObject(GLuint* renderingProgram);
	~GameObject();

	void initialize(int objectNumber, std::vector<GLuint>* VAO, std::vector<GLuint>* VBO, std::vector<GLuint>* EBO, std::vector<int>* meshVertexCounts,
		std::vector<int>* meshIndicesCount, std::vector<glm::vec3>* objectLocations, std::vector<float>* objectRotations);

	void Draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, GLuint VAO);

private:
	GLuint* renderingProgram;

	std::string basepath = "assets/";
};

