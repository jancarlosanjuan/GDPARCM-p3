#include "GameObjectManager.h"
#include "GameObject.h"

GameObjectManager* GameObjectManager::sharedInstance = nullptr;

GameObjectManager* GameObjectManager::getInstance()
{
    if (sharedInstance == nullptr)
        sharedInstance = new GameObjectManager();
    return sharedInstance;
}

GLuint* GameObjectManager::getRenderingProgram()
{
    return &renderingProgram;
}

void GameObjectManager::Draw()
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->Draw();
	}
}

void GameObjectManager::addGameObject(GameObject* newObject)
{
	gameObjects.push_back(newObject);
}

GameObjectManager::GameObjectManager()
{
}
