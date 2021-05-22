#pragma once
class IFinishedTask
{
public:
	virtual void OnFinished(int threadID) = 0;
};

