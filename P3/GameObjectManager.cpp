#include "GameObjectManager.h"

#include <iostream>

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

GLuint* GameObjectManager::getSkyboxRenderingProgram()
{
	return &skyboxRenderingProgram;
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

void GameObjectManager::removeGameObject(GameObject* newObject)
{
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), newObject), gameObjects.end());
}

GameObjectManager::GameObjectManager()
{
}
