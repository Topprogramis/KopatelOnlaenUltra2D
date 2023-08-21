#pragma once
#include"Command.hpp"

typedef std::vector<ICommand*> commandBuffer;

class CommandPoll {
public:
	CommandPoll() {
		m_buffers.push_back({});
		m_buffers.push_back({});
	}

	void Push(ICommand* command) {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_buffers[currentWriteBuffer.load(std::memory_order_relaxed)].push_back(command);
		lock.unlock();
	}
	bool TryPush(ICommand* command) {
		m_buffers[currentWriteBuffer.load(std::memory_order_relaxed)].push_back(command);
		return true;
	}

	void ExcuteAll() {
		std::vector<ICommand*> commands = m_buffers[currentReadBuffer.load(std::memory_order_relaxed)];
		m_buffers[currentReadBuffer.load(std::memory_order_relaxed)].clear();

		int readB = currentReadBuffer.load(std::memory_order_relaxed);
		int writeB = currentWriteBuffer.load(std::memory_order_relaxed);

		currentWriteBuffer.store(readB);
		currentReadBuffer.store(writeB);

	
		for (auto& i : commands) {
			i->Excute();
		}
	}
private:
	std::vector<commandBuffer> m_buffers;

	std::atomic<int> currentReadBuffer = 0, currentWriteBuffer = 1;

	std::mutex m_mutex;
};