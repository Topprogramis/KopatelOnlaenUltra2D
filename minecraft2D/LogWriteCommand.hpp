#pragma once
#include"Command.hpp"
#include"Logger.hpp"

class LogWriteCommand : public ICommand {
public:
	LogWriteCommand(Log log) {
		m_log = log;
	}

	void Excute() override {
		Logger::logger->Write(m_log);
	}
private:
	Log m_log;

};