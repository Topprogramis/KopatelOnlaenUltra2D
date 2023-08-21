#pragma once
#include"include.h"

class Log {
public:
	enum class LogType {
		error,
		successful,
		message,
		warring,
		any
	};

	Log(std::string msg = "empty", LogType type = LogType::message) {
		m_type = type;
		m_message = msg;
	}

	std::string getStr() {
		std::string typeStr;
		switch (m_type) {
		case LogType::error:
			typeStr = "ERROR";
			break;
		case LogType::successful:
			typeStr = "successful";
			break;
		case LogType::message:
			typeStr = "message";
			break;
		case LogType::warring:
			typeStr = "WARRING";
			break;
		case LogType::any:
			typeStr = "Any";
			break;
		}

		return  getNowTimeAsString("%H-%M-%S") + ":" + typeStr + ":" + m_message + "\n";
	}



private:
	LogType m_type;
	std::string m_message;
};

class Logger {
public:
	static Logger* logger;

	Logger() {
		

		logger = this;
	}
	void ReOpen() {
		

		m_fileName = "logs/log" + getNowTimeAsString("%Y-%m-%d %H-%M-%S") + ".txt";
	}
	void Write(Log log) {
		std::ofstream logF= std::ofstream(m_fileName, std::ios::app);

		logF << log.getStr();

		logF.close();
	}

private:
	std::string m_fileName;
};