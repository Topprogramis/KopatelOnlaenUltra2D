#pragma once
#include"Command.hpp"

class CommandPoll {
public:
	CommandPoll() {

	}

	void Push(ICommand* command) {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_commands.push_back(command);
		lock.unlock();
	}
	bool TryPush(ICommand* command) {
		if (!m_mutex.try_lock())
			return false;

		std::lock_guard<std::mutex> lock(m_mutex, std::adopt_lock);
		m_commands.push_back(command);
		return true;
	}

	void ExcuteAll() {
		std::unique_lock<std::mutex> lock(m_mutex);
		std::vector<ICommand*> commands = m_commands;
		m_commands = std::vector<ICommand*>{};
		lock.unlock();

		for (auto& i : commands) {
			i->Excute();
		}
	}
private:
	std::vector<ICommand*> m_commands;

	std::mutex m_mutex;
};