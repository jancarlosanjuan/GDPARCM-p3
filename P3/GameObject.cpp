#include "GameObject.h"


#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>


#include "GameObjectManager.h"
#include "ObjectLoader.h"
#include "tiny_obj_loader.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

void GameObject::initialize(std::string filePath, float r, float g, float b, glm::vec3 position, float rotation, GLuint texture)
{
	int objectNumber = GameObjectManager::getInstance()->getMeshVertexCounts()->size();

	VAO = 0;
	VBO = 0;
	EBO = 0;
	objectLocation = position;
	objectRotation = rotation;

	this->texture = texture;

	std::vector <tinyobj::shape_t> shapes = ObjectLoader::getInstance()->loadFile(filePath);

	meshVertexCount = shapes[0].mesh.positions.size();
	meshIndicesCount = shapes[0].mesh.indices.size();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.positions.size() * sizeof(float), &(shapes[0].mesh.positions[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "v_vertex"));
	glVertexAttribPointer(glGetAttribLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "v_vertex"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.normals.size() * sizeof(float), &(shapes[0].mesh.normals[0]), GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "v_normal"));
	glVertexAttribPointer(glGetAttribLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "v_normal"), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (shapes[0].mesh.texcoords.size() > 0) {
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.texcoords.size() * sizeof(float), &(shapes[0].mesh.texcoords[0]), GL_STATIC_DRAW);

		glEnableVertexAttribArray(glGetAttribLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "v_tex"));
		glVertexAttribPointer(glGetAttribLocation(*GameObjectManager::getInstance()->getRenderingProgram(), "v_tex"), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[0].mesh.indices.size() * sizeof(glm::uint), &(shapes[0].mesh.indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GameObject::Draw()
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);

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
