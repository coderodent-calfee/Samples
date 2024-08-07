#include "Tasks.h"
#include "CycleCheck.h"
#include <iterator> //for std::ostream_iterator
#include <algorithm> //for std::copy
#include <iostream> //for std::cout

using namespace std;
bool TaskManager::checkTasksForCycle() {
	CycleCheck cc;
	for (size_t id = 0; id < tasks_.size(); id++) {
		for (auto dependant : tasks_[id]->dependencies_) {
			cc.addEdge(dependant, id);
		}
	}
	return cc.checkCycle(tasks_.size(), 0);
}

void TaskManager::executeTasks() {
	std::vector<std::thread*> threads;

	std::map<int, std::shared_future<int>> futures;

	for (size_t id = 0; id < tasks_.size(); id++) {
		for (auto dependant : tasks_[id]->dependencies_) {
			if (futures.find(dependant) == futures.end()) {
				futures[dependant] = tasks_[dependant]->promise_.get_future();
				// shared_future provides a mechanism to access the result of asynchronous operations,
				// similar to std::future, except that multiple threads are allowed to wait for the same shared state
				// but we can only get it once
			}
			tasks_[id]->dependancyResults_.push_back(futures[dependant]);
		}
		std::thread* t = new std::thread(
			[=]() {	tasks_[id]->run(this);
			});
		threads.push_back(t);
	}
	for (auto t : threads) {
		t->join();
	}
}

void Task::run(TaskManager * mgr) {
	std::vector<int> results;
	{
		std::lock_guard<std::mutex> lock(mgr->mu_finished_);
		if (mgr->isFinished(index_)) {
			std::cerr << "This Task " << index_ << " has already been run" << endl;
			abort();
		}
		std::cout << "running task:" << index_ << " waiting on (" << dependancyResults_.size() << ") dependency results" << std::endl;
      std::cout << " waiting on: ";
		if (dependencies_.size()) {
			std::copy(
				dependencies_.begin(),
				dependencies_.end(),
				std::ostream_iterator<int>(std::cout, " ")
			);
		}
		else {
			std::cout << " nothing ";
		}
		std::cout << std::endl;
	}

	for (auto dependantResult : dependancyResults_) {
		// wait for dependant threads
		// The get member function waits (by calling wait()) until the shared state is ready, then retrieves the value stored in the shared state (if any).
		auto result = dependantResult.get();
		results.push_back(result);
		std::cout << " task " << index_ << " is dependant on: ";
		if (dependencies_.size()) {
			std::copy(
				dependencies_.begin(),
				dependencies_.end(),
				std::ostream_iterator<int>(std::cout, " ")
			);
		}
		else {
			std::cout << " nothing ";
		}
		std::cout << std::endl;
		std::cout << " task " << index_ << " dependency result:" << result << std::endl;

	}

	{
		std::lock_guard<std::mutex> lock(mgr->mu_finished_);
		for (int dependantTask : dependencies_) {
			if (!mgr->isFinished(dependantTask)) {
				std::cerr << "Task " << index_ << " depends on task " << dependantTask << " which isn't finished" << endl;
				abort();
			}
		}
		cout << "Starting task " << index_ << "\n";
	}

	doWork_(results);

	{
		std::lock_guard<std::mutex> lock(mgr->mu_finished_);
		mgr->finish(index_);
		cout << "Ran task " << index_ << " with result " << index_ << std::endl;
		promise_.set_value(index_);
	}
}
