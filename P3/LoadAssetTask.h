#pragma once
#include "IWorkerAction.h"
#include <string>

class IExecutionEvent;
class LoadAssetTask : public IWorkerAction
{
	typedef std::string String;
public:
	LoadAssetTask(String path, IExecutionEvent* execEvent);
	~LoadAssetTask();
	
	void OnStartTask() override;

private:
	String path;
	IExecutionEvent* execEvent;
};

