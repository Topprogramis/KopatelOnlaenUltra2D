#pragma once
#include"include.h"
class UiObject {
private:
	static sf::RenderWindow* m_window;
};

sf::RenderWindow* UiObject::m_window = nullptr;