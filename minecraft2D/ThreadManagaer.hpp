#pragma once
#include"include.h";
#include"Thread.hpp"

class ThreadManager {
public:
	ThreadManager() {

	}

	void LaunchAll() {
		for (auto& i : m_threads) {
			i->Launch();
		}
	}

	int AddThread(Thread* thread) {
		m_threads.push_back(thread);

		m_threadIds[thread->getName()] = m_threads.size() - 1;

		return m_threads.size() - 1;
	}

	void AddCommand(std::string name, ICommand* command) {
		GetThreadByName(name)->AddToEventPoll(command);
	}
private:
	std::vector<Thread*> m_threads = {};
	std::map<std::string, int> m_threadIds = {};

	Thread* GetThreadByName(std::string name) {
		return m_threads[m_threadIds[name]];
	}
};