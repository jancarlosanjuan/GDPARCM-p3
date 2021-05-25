#include "GameObject.h"

#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>

#include "GameObjectManager.h"
#include "ObjectLoader.h"
#include <unordered_map>

std::unordered_map<GameObject*, std::vector<tinyobj::shape_t>> shapes;

GameObject::GameObject()
{

}

GameObject::~GameObject()
{
}

void GameObject::getMesh(std::string filepath)
{
	shapes.insert(std::pair<GameObject*, std::vector<tinyobj::shape_t>>(this, ObjectLoader::getInstance()->loadFile(filepath)));
}

void GameObject::initialize(float r, float g, float b, glm::vec3 position, float rotation)
{
	VAO = 0;
	VBO = 0;
	EBO = 0;
	objectLocation = position;
	objectRotation = rotation;

	std::vector<tinyobj::shape_t> shape = shapes[this];

	meshVertexCount = shape[0].mesh.positions.size();
	meshIndicesCount = shape[0].mesh.indices.size();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, shape[0].mesh.positions.size() * sizeof(float), &(shape[0].mesh.positions[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "v_vertex"));
	glVertexAttribPointer(glGetAttribLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "v_vertex"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, shape[0].mesh.normals.size() * sizeof(float), &(shape[0].mesh.normals[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "v_normal"));
	glVertexAttribPointer(glGetAttribLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "v_normal"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (shape[0].mesh.texcoords.size() > 0) {
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, shape[0].mesh.texcoords.size() * sizeof(float), &(shape[0].mesh.texcoords[0]), GL_STATIC_DRAW);

		glEnableVertexAttribArray(glGetAttribLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "v_tex"));
		glVertexAttribPointer(glGetAttribLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "v_tex"), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape[0].mesh.indices.size() * sizeof(glm::uint), &(shape[0].mesh.indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GameObject::Draw()
{
	glBindVertexArray(VAO);

	GLuint model_loc = glGetUniformLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "u_model");
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, objectLocation);
	model = glm::rotate(model, objectRotation, glm::vec3(0, 1, 0));
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);//rebind indices buffer from init() before drawing
	glDrawElements(GL_TRIANGLES, meshIndicesCount, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);//unbind
}
