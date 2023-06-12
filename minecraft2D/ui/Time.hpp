#pragma once
#include "../include.h"

class Time {
public:
	static float deltaTime;

	Time() {
		m_timer = sf::Clock();
	}
	~Time() {

	}

	void Update() {
		deltaTime = m_timer.getElapsedTime().asSeconds();
		m_timer.restart();
	}
private:
	sf::Clock m_timer;
};

float Time::deltaTime = 0.0f;