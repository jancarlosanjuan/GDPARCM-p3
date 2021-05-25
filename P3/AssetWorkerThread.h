#pragma once
#include "IETThread.h"
#include <string>

class GameObject;
class IExecutionEvent;
class AssetWorkerThread : public IETThread
{
public:
	AssetWorkerThread(std::string modelPath, GameObject* object, IExecutionEvent* execEvent);
	~AssetWorkerThread();

private:

	void run() override;
	
	std::string modelPath;
	GameObject* loadingObject;
	IExecutionEvent* execEvent;
};

