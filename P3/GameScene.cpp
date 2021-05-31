#include "GameScene.h"
#include "GameObject.h"
#include "GameObjectManager.h"
#include "loadObjectMesh.h"
#include "ThreadPool.h"

GameScene::GameScene(std::vector<std::string> models, ThreadPool* pool)
{
	this->modelPaths = models;
	this->pool = pool;
}

GameScene::~GameScene()
{
}

void GameScene::loadScene()
{
	for (int i = 0; i < modelPaths.size(); i++)
	{
		GameObject* newObject = new GameObject();
		gameObjects.push_back(newObject);
		loadObjectMesh* task = new loadObjectMesh(modelPaths[i], newObject, this);
		pool->scheduleTask(task);
	}
}

void GameScene::activateScene()
{
	if (loadedObjectsNum == modelPaths.size() && !displayingObjects) 
	{
		showObjects();
	}
}

void GameScene::deactivateScene()
{
	if (displayingObjects) {
		for (int i = 0; i < gameObjects.size(); i++)
		{
			GameObjectManager::getInstance()->removeGameObject(gameObjects[i]);
		}
	}

	displayingObjects = false;
}

void GameScene::unloadScene()
{
	if (initializedObjects) {
		if (displayingObjects)
		{
			for (int i = 0; i < gameObjects.size(); i++)
			{
				GameObjectManager::getInstance()->removeGameObject(gameObjects[i]);
			}
		}

		for (int i = 0; i < gameObjects.size(); i++)
		{
			delete gameObjects[i];
		}
		gameObjects.clear();

		initializedObjects = false;
		loadedObjectsNum = 0;
		displayingObjects = false;
	}
	else if (loadedObjectsNum == modelPaths.size())
	{
		for (int i = 0; i < gameObjects.size(); i++)
		{
			delete gameObjects[i];
		}
		gameObjects.clear();

		initializedObjects = false;
		loadedObjectsNum = 0;
		displayingObjects = false;
	}
}

float GameScene::getLoadProgress()
{
	return (float)loadedObjectsNum / modelPaths.size();
}

void GameScene::showObjects()
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (!initializedObjects)
			gameObjects[i]->initialize(glm::vec3(-50 + rand() % 101, 10, -75 + rand() % 50), 0);
		GameObjectManager::getInstance()->addGameObject(gameObjects[i]);
	}
	initializedObjects = true;
	displayingObjects = true;
}

void GameScene::OnFinishedExecution()
{
	loadedObjectsNum++;
}
