#pragma once
#include"UiElement.hpp"

enum class TextOutOfBoundType {
	Scale,
	Scroll,
	NextLine
};

enum class TextHorizontalSpaceType {
	Left,
	Right,
	Centre
};

enum class TextVerticalSpaceType {
	Bottom,
	Top,
	Centre
};


class TextBlock : public UiElement {
public:
	TextBlock(sf::Font* font = nullptr,sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2f size = sf::Vector2f(10, 10), std::string text = "", float charectSize = 36, sf::Color fillColor = sf::Color::White, std::string name = "", bool active = true) : UiElement(pos, size,name, active) {
		m_str = text;
		m_fillColor = fillColor;
		m_charectSize = charectSize;
		if (font) {
			m_font = *font;
			m_tx = sf::Text(m_str, m_font);

			m_lenght = 0;

			m_reallyCharectoeSize = m_tx.getFont()->getGlyph('a', m_charectSize, false).bounds.width;


			UpdateText();
		}
	}

	void Str(std::string str) {
		m_str = str;
		UpdateText();
	}
	std::string Str() { return m_str; }

	void CharecterSize(float size) {
		m_charectSize = size;
	}
	float CharecterSize() { return m_charectSize; }

	void FillColor(sf::Color fillColor) {
		m_fillColor = fillColor;
		UpdateText();
	}
	sf::Color FillColor() { return m_fillColor; }

	void OutOfBoundType(TextOutOfBoundType textOutOfBoundType) {
		m_outOfBoundsType = textOutOfBoundType;
		UpdateText();
	}
	TextOutOfBoundType OutOfBoundType() { return m_outOfBoundsType; }

	void HorizontalSpaceType(TextHorizontalSpaceType horizontalSpaceType) {
		m_horizontalSpaceType = horizontalSpaceType;
		UpdateText();
	}
	TextHorizontalSpaceType HorizontalSpaceType() { return m_horizontalSpaceType; }

	void VerticalSpaceType(TextVerticalSpaceType verticalSpaceType) {
		m_verticalSpaceType = verticalSpaceType;
		UpdateText();
	}
	TextVerticalSpaceType VerticalSpaceType() { return m_verticalSpaceType; }

	void Font(sf::Font* font) {
		m_font = *font;
		m_tx = sf::Text(m_str, m_font);

		m_lenght = 0;

		m_reallyCharectoeSize = m_tx.getFont()->getGlyph('a', m_charectSize, false).bounds.width;


		UpdateText();
	}

	void Font(std::string str) {
		m_font.loadFromFile(str);
		m_tx = sf::Text(m_str, m_font);

		m_lenght = 0;

		m_reallyCharectoeSize = m_tx.getFont()->getGlyph('a', m_charectSize, false).bounds.width;


		UpdateText();
	}

	bool LoadFromXmlNode(pugi::xml_node node) {
		UiElement::LoadFromXmlNode(node);
		if (node.attribute("Font").as_string()  != "") {
			std::string font = node.attribute("Font").as_string();
			Font(font);
		}
		if (node.attribute("CharectSize").as_string()  != "") {
			float charectSize = node.attribute("CharectSize").as_float();
			CharecterSize(charectSize);
		}
		if (node.attribute("Text").as_string()  != "") {
			std::string text = node.attribute("Text").as_string();
			Str(text);
		}
		if (node.attribute("FillColor").as_string() != "") {

			sf::Color fillColor = ColorFromString(node.attribute("Color").as_string());
			FillColor(fillColor);
		}
		if (node.attribute("HorizontalBind").as_string() != "") {
			std::string HBindType = node.attribute("HorizontalBind").as_string();
			if (HBindType == "Left") {
				HorizontalSpaceType(TextHorizontalSpaceType::Left);
			}
			else if (HBindType == "Right") {
				HorizontalSpaceType(TextHorizontalSpaceType::Right);
			}
			else if (HBindType == "Center") {
				HorizontalSpaceType(TextHorizontalSpaceType::Centre);
			}
		}
		if (node.attribute("VerticalBind").as_string() != "") {
			std::string HBindType = node.attribute("VerticalBind").as_string();
			if (HBindType == "Top") {
				VerticalSpaceType(TextVerticalSpaceType::Top);
			}
			else if (HBindType == "Buttom") {
				VerticalSpaceType(TextVerticalSpaceType::Bottom);
			}
			else if (HBindType == "Center") {
				VerticalSpaceType(TextVerticalSpaceType::Centre);
			}
		}
		if (node.attribute("OutOfBound").as_string() != "") {
			std::string HBindType = node.attribute("OutOfBound").as_string();
			if (HBindType == "NextLine") {
				OutOfBoundType(TextOutOfBoundType::NextLine);
			}
			else if (HBindType == "Scroll") {
				OutOfBoundType(TextOutOfBoundType::Scroll);
			}
			else if (HBindType == "Scale") {
				OutOfBoundType(TextOutOfBoundType::Scale);
			}
		}

		return true;
	}

	void UpdateRealyFontSize();

	void Draw(sf::RenderTexture* tx) override {
		m_tx.setPosition(m_transform.LocalPosition() + m_offset);
		tx->draw(m_tx);
		UiElement::Draw(tx);
	}

protected:
	sf::Vector2f m_offset=sf::Vector2f(0,0);
	std::string m_str;
	float m_charectSize,m_reallyCharectoeSize,m_lenght;
	sf::Color m_fillColor;
	sf::Text m_tx;
	sf::Font m_font;


	TextOutOfBoundType m_outOfBoundsType = TextOutOfBoundType::NextLine;
	TextHorizontalSpaceType m_horizontalSpaceType = TextHorizontalSpaceType::Centre;
	TextVerticalSpaceType m_verticalSpaceType = TextVerticalSpaceType::Centre;

	void UpdateText();
};