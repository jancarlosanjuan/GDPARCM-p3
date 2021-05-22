#pragma once
#include "IETThread.h"

class IExecutionEvent;
class Skybox;
class boxLoader : public IETThread
{
public:
	boxLoader(IExecutionEvent* execEvent, Skybox* skybox);
	~boxLoader();

private:
	void run() override;
	IExecutionEvent* execEvent;
	Skybox* skybox;
};

