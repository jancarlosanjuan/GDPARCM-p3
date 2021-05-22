#include "PoolWorkerThread.h"
#include "IWorkerAction.h"
#include "IFinishedTask.h"

PoolWorkerThread::PoolWorkerThread(int id, IFinishedTask* finishedTask)
{
	this->id = id;
	this->finishedTask = finishedTask;
}

PoolWorkerThread::~PoolWorkerThread()
{
}

int PoolWorkerThread::getThreadID()
{
	return this->id;
}

void PoolWorkerThread::assignTask(IWorkerAction* action)
{
	this->action = action;
}

void PoolWorkerThread::run()
{
	this->action->OnStartTask();
	this->finishedTask->OnFinished(this->id);
}
