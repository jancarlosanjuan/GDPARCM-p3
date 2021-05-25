#include "AssetWorkerThread.h"
#include "GameObject.h"
#include "IExecutionEvent.h"

AssetWorkerThread::AssetWorkerThread(std::string modelPath, GameObject* object, IExecutionEvent* execEvent)
{
	this->modelPath = modelPath;
	this->loadingObject = object;
	this->execEvent = execEvent;
}

AssetWorkerThread::~AssetWorkerThread()
{
}

void AssetWorkerThread::run()
{
	loadingObject->getMesh(modelPath);
	execEvent->OnFinishedExecution();
	delete this;
}
