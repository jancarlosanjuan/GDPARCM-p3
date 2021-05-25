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
		gameObjects[i]->initialize(1.0f, 1.0f, 1.0f, glm::vec3(-50.0f, 0, -50.0f), 0);
		GameObjectManager::getInstance()->addGameObject(gameObjects[i]);
	}
	displayingObjects = true;
}

void GameScene::OnFinishedExecution(GameObject* loadedObject)
{
	loadedObjectsNum++;
}
