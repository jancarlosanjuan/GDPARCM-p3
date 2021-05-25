#pragma once
#include <vector>
#include <string>
#include "IExecutionEvent.h"

class GameObject;
class ThreadPool;
class GameScene : public IExecutionEvent
{
public:
	GameScene(std::vector<std::string> models, ThreadPool* pool);
	~GameScene();

	void loadScene();
	void activateScene();
	void deactivateScene();

	float getLoadProgress();

	void showObjects();
	
	void OnFinishedExecution() override;

private:
	std::vector<GameObject*> gameObjects;
	std::vector<std::string> modelPaths;
	int loadedObjectsNum = 0;

	bool initializedObjects = false;
	bool displayingObjects = false;

	ThreadPool* pool;
};

