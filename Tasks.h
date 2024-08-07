#pragma once

#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <thread>
#include <utility>
#include <vector>

// TODO: supply the work to do in 'doWork' as a lambda


class Task;

class TaskManager {
public:
	TaskManager(const std::vector<Task*>& tasks) : tasks_(tasks) {}

	bool doTasks() 
	{
		std::cout << "\n\ncheckTasksForCycle:\n";
		if (checkTasksForCycle()) {
			std::cout << "YES Found a cycle, tasks not executed\n";
			return false;
		}
		else
		{
			std::cout << "NO Cycle, executing tasks...\n";
			executeTasks();
		}
		std::cout << "Done\n";
		return true;
	}
	bool isFinished(int index) {
		return finished_[index];
	}
	void finish(int index) {
		finished_[index] = true;
	}
	bool checkTasksForCycle();

	std::mutex mu_finished_;

protected:
	void executeTasks();

private:
	std::vector<Task*> tasks_;
	std::map<int, bool> finished_;
}; // TaskManager


class Task {
public:
	Task(int index, std::set<int> dependencies)
		: index_(index), dependencies_(dependencies) {}

	// Task is non-movable and non-copyable
	Task(const Task&) = delete;
	Task(Task&&) = delete;
	Task& operator=(const Task&) = delete;
	Task& operator=(Task&&) = delete;

	void run(TaskManager*);
	std::promise<int> promise_;
	const int index_;
	const std::set<int> dependencies_;
	std::vector <std::shared_future<int>> dependancyResults_;

	void setWork(std::function<void(const  std::vector <int>& f)> doWork_);
private:
	std::function<void(const  std::vector <int>&)> doWork_ = [](const std::vector <int> &) {
		const auto end_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
		while (std::chrono::steady_clock::now() < end_time) {}
	};
}; // Task



