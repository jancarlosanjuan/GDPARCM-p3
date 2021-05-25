#pragma once

#include <string>

#include "IExecutionEvent.h"

class Game
{
public:
	Game();
	~Game();

	void Run();

private:
	void init();
	void display();

	void calculateLighting();

	std::string basepath = "assets/";
	std::string treeTrunkFile = basepath + "treetrunk.obj";
	std::string fallenFile = basepath + "fallentree.obj";
	std::string houseFile = basepath + "house.obj";
	std::string grassFile = basepath + "grass.obj";
	std::string groundFile = basepath + "ground.obj";

	class Skybox* skybox;
	class loadWaiter* waiter;
};

class loadWaiter : public IExecutionEvent
{
public:
	bool finishedLoading = false;
	void OnFinishedExecution(GameObject* loadedObject) override;
};