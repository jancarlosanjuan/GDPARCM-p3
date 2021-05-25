#include "loadObjectMesh.h"
#include "IExecutionEvent.h"
#include "GameObject.h"

loadObjectMesh::loadObjectMesh(std::string modelPath, GameObject* object, IExecutionEvent* execEvent)
{
	this->modelPath = modelPath;
	this->loadingObject = object;
	this->execEvent = execEvent;
}

loadObjectMesh::~loadObjectMesh()
{
}

void loadObjectMesh::OnStartTask()
{
	loadingObject->getMesh(modelPath);
	execEvent->OnFinishedExecution();
	delete this;
}
