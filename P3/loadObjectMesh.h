#pragma once
#include <string>

#include "IWorkerAction.h"

class GameObject;
class IExecutionEvent;
class loadObjectMesh : public IWorkerAction
{
public:
	loadObjectMesh(std::string modelPath, GameObject* object, IExecutionEvent* execEvent);
	~loadObjectMesh();
	
	void OnStartTask() override;

private:
	std::string modelPath;
	GameObject* loadingObject;
	IExecutionEvent* execEvent;
};

