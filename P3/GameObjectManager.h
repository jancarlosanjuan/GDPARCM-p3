#pragma once
#include <vector>
#include <GL/glew.h>

class GameObject;
class GameObjectManager
{
public:
	static GameObjectManager* getInstance();

	GLuint* getRenderingProgram();

	void Draw();
	void addGameObject(GameObject* newObject);
	
private:
	GameObjectManager();
	GameObjectManager(GameObjectManager const&) {};             // copy constructor is private
	GameObjectManager& operator=(GameObjectManager const&) {};  // assignment operator is private
	static GameObjectManager* sharedInstance;

	std::vector<GameObject*> gameObjects;

	GLuint renderingProgram;
};

