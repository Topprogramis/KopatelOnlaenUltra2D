#pragma once
#include"include.h"
#include"CommandPoll.hpp"

class Thread {
public:
	Thread(std::string name, std::function<void(Thread*)> func) {
		m_name = name;
		m_thread = new std::thread(func,this );
	}

	void ResetClock() {
		m_clock.restart();
	}
	float getElapseTimeAsSeconds() {
		return m_clock.getElapsedTime().asSeconds();
	}
	float getElapseTime() {
		return m_clock.getElapsedTime().asMilliseconds();
	}

	void Launch() {
		m_thread->join();
	}

	void AddToEventPoll(ICommand* command) {
		m_commandPoll.Push(command);
	}
	void ExcuteCommands() {
		m_commandPoll.ExcuteAll();
	}

	std::string getName() {
		return m_name;
	}
private:
	std::string m_name;

	std::thread* m_thread;
	sf::Clock m_clock;

	CommandPoll m_commandPoll;
};