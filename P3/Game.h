#pragma once
#include "IExecutionEvent.h"

class Game
{
public:
	Game();
	~Game();

	void Run();
};

class loadWaiter : public IExecutionEvent
{
public:
	bool finishedLoading = false;
	void OnFinishedExecution() override;
};