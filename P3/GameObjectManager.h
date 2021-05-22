#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/vec3.hpp>

class GameObjectManager
{
public:
	static GameObjectManager* getInstance();

	std::vector<GLuint>* getVBO();
	std::vector<GLuint>* getVAO();
	std::vector<GLuint>* getEBO();

	std::vector <int>* getMeshVertexCounts();
	std::vector <int>* getMeshIndicesCount();
	std::vector <glm::vec3>* getObjectLocations();
	std::vector <float>* getObjectRotations();
private:
	GameObjectManager();
	GameObjectManager(GameObjectManager const&) {};             // copy constructor is private
	GameObjectManager& operator=(GameObjectManager const&) {};  // assignment operator is private
	static GameObjectManager* sharedInstance;

	std::vector<GLuint> VBO, VAO, EBO;//buffers for all objects

	std::vector <int> meshVertexCounts;
	std::vector <int> meshIndicesCount;
	std::vector <glm::vec3> objectLocations;
	std::vector <float> objectRotations;
};

