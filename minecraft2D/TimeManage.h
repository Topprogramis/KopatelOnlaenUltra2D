#pragma once
#include"include.h"

class Time {
public:
	Time();

	void Update();
	float DeltaTime();

	static Time* globalTimeManager;

private:
	float m_deltaTime = 0.0f;

	sf::Clock cl;
};