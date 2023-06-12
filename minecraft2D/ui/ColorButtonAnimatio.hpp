#pragma once
#include"ButtonAnimation.hpp"
#include"Convertor.h"

class ColorButtonAnimation : public ButtonAnimation {
public:
	ColorButtonAnimation(float time,sf::Color pressedColor,sf::Color standartColor,sf::Color focusColor) : ButtonAnimation(time) {
		m_pressedColor = pressedColor;
		m_standartColor = standartColor;
		m_focusColor = focusColor;

		m_color = glm::vec4(m_standartColor.r, m_standartColor.g, m_standartColor.b, m_standartColor.a);
	}


	static ColorButtonAnimation* loadFromXmlNode(pugi::xml_node node) {
		sf::Color pressedColor, standartColor, focusColor;
		float time;
		if (node.attribute("PressedColor").as_string() != "") {
			pressedColor = ColorFromString(node.attribute("PressedColor").as_string());
		}
		if (node.attribute("FocusColor").as_string() != "") {
			focusColor = ColorFromString(node.attribute("FocusColor").as_string());
		}
		if (node.attribute("StandartColor").as_string() != "") {
			standartColor = ColorFromString(node.attribute("StandartColor").as_string());
		}
		if (node.attribute("Time").as_string() != "") {
			time = node.attribute("Time").as_float();
		}

		return new ColorButtonAnimation(time, pressedColor, standartColor, focusColor);
		
	}

	void ProcessAnim() override;

protected:
	void UpdateNeedColor();

	sf::Color m_pressedColor, m_standartColor, m_focusColor;
	glm::vec4 m_color;

	glm::vec4 m_needColor, m_step;

	

};