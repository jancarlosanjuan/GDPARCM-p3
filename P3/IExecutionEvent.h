#pragma once
class GameObject;
class IExecutionEvent
{
public:
	virtual void OnFinishedExecution(GameObject*) = 0;
};

