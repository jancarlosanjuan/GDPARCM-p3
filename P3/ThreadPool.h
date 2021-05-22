#pragma once
#include <string>
#include <queue>
#include <unordered_map>
#include "IFinishedTask.h"
#include "IETThread.h"

class PoolWorkerThread;
class IWorkerAction;
class IExecutionEvent;

class ThreadPool: public IFinishedTask, public IETThread
{
private:
	typedef std::string String;
	typedef std::queue<PoolWorkerThread*> ThreadList;
	typedef std::unordered_map<int, PoolWorkerThread*> ActiveThreadMap;
	typedef std::queue<IWorkerAction*> ActionList;

public:
	ThreadPool(String name, int numWorkers);
	~ThreadPool();

	void startScheduler();
	void stopScheduler();
	void scheduleTask(IWorkerAction* action);
	
private:
	String name;
	int numWorkers = 0;
	bool running = false;

	ActiveThreadMap activeThreads;
	ThreadList inactiveThreads;
	ActionList pendingActions;

	void OnFinished(int threadID) override;
	void run() override;
};

