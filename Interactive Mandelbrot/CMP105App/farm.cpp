#include "farm.h"
#include <iostream>

void Farm::add_task(Task *task)
{
	// Add task to the queue.
	taskQueue.push(task);
}


void Farm::run()
{
	// Thread function that safely gets the first task in the queue using a mutex, runs it and then deletes it. This repeats until there are no tasks remaining.
	auto threadFunction = [&]()
	{
		while (!taskQueue.empty())
		{
				queueMutex.lock();
				Task* task = taskQueue.front();
				taskQueue.pop();
				queueMutex.unlock();
				task->run();
				delete task;
		}
	};

	// Create threads using above thread function.
	for (int i = 0; i < threadCount; i++)
	{
		threads.push_back(new std::thread(threadFunction));
	}

	// Wait for all of the threads to finish, and then delete them.
	for (int i = 0; i < threadCount; i++)
	{
		threads[i]->join();
		delete threads[i];
	}
}


