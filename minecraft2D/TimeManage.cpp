#include"TimeManage.h"


Time::Time() {

}

void Time::Update() {
	m_deltaTime = cl.getElapsedTime().asSeconds();
	cl.restart();
}

float Time::DeltaTime() {
	return m_deltaTime;
}