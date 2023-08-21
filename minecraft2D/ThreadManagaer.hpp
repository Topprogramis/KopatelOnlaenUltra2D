#pragma once
#include"include.h";
#include"Thread.hpp"
#include"LogWriteCommand.hpp"

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
	Thread* getThread(std::string name) {
		return m_threads[m_threadIds[name]];
	}

	void AddCommand(std::string name, ICommand* command) {
		GetThreadByName(name)->AddToEventPoll(command);
	}

	bool TryAddCommand(std::string name, ICommand* command) {
		return GetThreadByName(name)->TryAddToEventPoll(command);
	}
private:
	std::vector<Thread*> m_threads = {};
	std::map<std::string, int> m_threadIds = {};

	Thread* GetThreadByName(std::string name) {
		return m_threads[m_threadIds[name]];
	}
};

class ThreadBarier {
public:
	void CompliteWork() {
		m_compliteWorkThreadCount.fetch_add(1, std::memory_order_relaxed);


		m_conv.notify_one();
	}
	void CompliteAndLock() {
		m_compliteWorkThreadCount++;
		//m_conv.notify_one();
	}
	void Wait(int count, int timeOut = 10000) {
		std::unique_lock<std::mutex> lock(m_mutex);
		if (!m_conv.wait_until(lock, std::chrono::system_clock::now() + std::chrono::milliseconds(timeOut), [&, this]() {return m_compliteWorkThreadCount.load(std::memory_order_relaxed) >= count; })) {
			Settings::threadManager->AddCommand("log", new LogWriteCommand(Log("Thread time out", Log::LogType::warring)));
		}
		lock.unlock();

		m_compliteWorkThreadCount.store(0, std::memory_order_relaxed);
	}
private:
	std::mutex m_mutex;
	std::atomic<int> m_compliteWorkThreadCount=0;

	std::condition_variable m_conv;
};

class ThreadLock {
public:
	void UnLock() {
		m_conv.notify_one();
	}

	void Wait() {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_conv.wait(lock);
		lock.unlock();
	}
private:
	std::mutex m_mutex;

	std::condition_variable m_conv;
};