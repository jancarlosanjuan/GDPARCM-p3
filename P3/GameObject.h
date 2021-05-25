#pragma once
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>

class GameObject
{
public:
	GameObject();
	~GameObject();

	void getMesh(std::string filepath);
	void initialize(float r, float g, float b, glm::vec3 position, float rotation);

	void Draw();

private:
	GLuint VBO, VAO, EBO;

	int meshVertexCount;
	int meshIndicesCount;
	glm::vec3 objectLocation;
	float objectRotation;
};

