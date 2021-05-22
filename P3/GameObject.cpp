#include "GameObject.h"

GameObject::GameObject(GLuint* renderingProgram)
{
	this->renderingProgram = renderingProgram;
}

GameObject::~GameObject()
{
}

void GameObject::initialize(int objectNumber, std::vector<GLuint>* VAO, std::vector<GLuint>* VBO,
	std::vector<GLuint>* EBO, std::vector<int>* meshVertexCounts, std::vector<int>* meshIndicesCount,
	std::vector<glm::vec3>* objectLocations, std::vector<float>* objectRotations)
{
	
}

void GameObject::Draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, GLuint VAO)
{
}
