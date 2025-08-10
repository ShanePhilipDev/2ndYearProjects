#ifndef FARM_H
#define FARM_H

#include "task.h"
#include <queue>
#include <mutex>
#include <thread>

/** A collection of tasks that should be performed in parallel. */
class Farm {
public:
	/** Add a task to the farm.
	    The task will be deleted once it has been run. */
	void add_task(Task *task);

	/** Run all the tasks in the farm.
	    This method only returns once all the tasks in the farm
		have been completed. */
	void run();

	// Set number of threads that will be used in the farm.
	void set_threads(int x)
	{
		threadCount = x;
	}
private:
	// A queue for holding all of the tasks, and a mutex to prevent more than one thread from accessing the queue at a time.
	std::queue<Task*> taskQueue;
	std::mutex queueMutex;

	// A vector to store the threads and an integer to specify how many threads are to be created.
	std::vector<std::thread*> threads;
	int threadCount;
};

#endif
