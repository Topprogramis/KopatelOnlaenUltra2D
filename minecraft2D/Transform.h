#pragma once
#include"include.h"
class Transform
{
public:
	Transform(sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2f size = sf::Vector2f(1, 1));

	void SetPostion(sf::Vector2f pos);
	sf::Vector2f GetPosition();

	void Move(sf::Vector2f offset);

	void SetSize(sf::Vector2f pos);
	sf::Vector2f GetSize();

private:
	sf::Vector2f m_pos, m_size;
};

