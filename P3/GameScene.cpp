#include "GameScene.h"
#include "GameObject.h"
#include "GameObjectManager.h"
#include "AssetWorkerThread.h"

GameScene::GameScene(std::vector<std::string> models)
{
	this->modelPaths = models;
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
		AssetWorkerThread* worker = new AssetWorkerThread(modelPaths[i], newObject, this);
		worker->start();
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
	for (int i = 0; i < gameObjects.size(); i++)
	{
		GameObjectManager::getInstance()->removeGameObject(gameObjects[i]);
	}

	displayingObjects = false;
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
