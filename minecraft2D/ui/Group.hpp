#pragma once
#include"UiElement.hpp"

class Group: public UiElement {
public:
	Group(sf::Vector2f pos = sf::Vector2f(0,0), sf::Vector2f size = sf::Vector2f(0, 0), std::string name = "", bool active = true) : UiElement(pos, size, name,active) {
		
	}
};