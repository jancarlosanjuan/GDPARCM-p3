#pragma once
#include <vector>
#include <string>
#include "IExecutionEvent.h"

class GameObject;
class GameScene : public IExecutionEvent
{
public:
	GameScene(std::vector<std::string> models);
	~GameScene();

	void loadScene();
	void activateScene();
	void deactivateScene();

	void showObjects();
	
	void OnFinishedExecution() override;

private:
	std::vector<GameObject*> gameObjects;
	std::vector<std::string> modelPaths;
	int loadedObjectsNum = 0;

	bool initializedObjects = false;
	bool displayingObjects = false;
};

