#include "ModuleTaskManager.h"


void ModuleTaskManager::threadMain()
{
	// TODO 3:
		// - Wait for new tasks to arrive
		// - Retrieve a task from scheduledTasks
		// - Execute it
		// - Insert it into finishedTasks
	Task *t = nullptr;
	while (true)
	{
		{
			std::unique_lock<std::mutex> lock(mtx);
			while (scheduledTasks.empty() && !exitFlag)
			{
				event.wait(lock);
			}
			if (exitFlag)
			{
				break;
			}
			else
			{
				t = scheduledTasks.front();
				scheduledTasks.pop();
			}
		}
		t->execute();
		{
			std::unique_lock<std::mutex> lock(mtx);
			finishedTasks.push(t);
		}
	}
	
}

bool ModuleTaskManager::init()
{
	// TODO 1: Create threads (they have to execute threadMain())
	for (int i = 0; i < MAX_THREADS; ++i)
	{
		threads[i] = std::thread(&ModuleTaskManager::threadMain,this);
	}

	return true;
}

bool ModuleTaskManager::update()
{
	// TODO 4: Dispatch all finished tasks to their owner module (use Module::onTaskFinished() callback)
	std::unique_lock<std::mutex> lock(mtx);
	while (!finishedTasks.empty())
	{
		finishedTasks.front()->owner->onTaskFinished(finishedTasks.front());
		finishedTasks.pop();
	}

	return true;
}

bool ModuleTaskManager::cleanUp()
{
	// TODO 5: Notify all threads to finish and join them
	{
		std::unique_lock<std::mutex> lock(mtx);
		exitFlag = true;
		event.notify_all();
	}
	

	for (int i = 0; i < MAX_THREADS; ++i)
	{
		threads[i].join();
	}

	return true;
}

void ModuleTaskManager::scheduleTask(Task *task, Module *owner)
{
	task->owner = owner;

	// TODO 2: Insert the task into scheduledTasks so it is executed by some thread
	{
		std::unique_lock<std::mutex> lock(mtx);
		scheduledTasks.push(task);
		event.notify_one();
	}
	
}


//#include <stdlib.h>
//#include <thread>
//#include <iostream>
//#include <windows.h>
//#include <mutex>
//
//
//
//int counter = 0; // Global var
//std::mutex mtx;
//
//void increment(int iterations) {
//	for (int i = 0; i < iterations; ++i) {
//		std::unique_lock<std::mutex> lock(mtx);
//		counter++;
//	}
//}
//int main() {
//	std::thread t1(increment, 100000); // Execute 'increment' in a thread
//	std::thread t2(increment, 100000); // Execute 'increment' in a thread
//
//	t1.join(); // Wait for t1
//	t2.join(); // Wait for t2
//	std::cout << "Counter = " << counter << std::endl;
//
//	system("pause");
//	return 0;
//}
//
//void taskManagerMain(int arg1, int arg2, int arg3)
//{
//	while (true)
//	{
//		Task *t = nullptr;
//
//
//		std::unique_lock<std::mutex> lock(mtx);
//		while (t == nullptr)
//		{
//
//			if (scheduledTasks.empty())
//			{
//				cond.wait(lock);
//			}
//			else
//			{
//				t = scheduledTasks.front();
//				scheduledTasks.pop();
//			}
//		}
//	}
//
//}