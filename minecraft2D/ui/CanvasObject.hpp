#pragma once
#include"UiElement.hpp"

class CanvasObject : public UiElement{
public:
	CanvasObject(sf::Vector2f pos = { 100,100 }, sf::Vector2f size = {100,100}, std::string name = "CanvasObject", bool active = true) : UiElement(pos, size, name, active) {

	}

};