#include "boxLoader.h"
#include "IExecutionEvent.h"
#include "Skybox.h"

boxLoader::boxLoader(IExecutionEvent* execEvent, Skybox* skybox)
{
	this->execEvent = execEvent;
	this->skybox = skybox;
}

boxLoader::~boxLoader()
{
}

void boxLoader::run()
{
	skybox->createShader();
	this->execEvent->OnFinishedExecution();
	delete this;
}
