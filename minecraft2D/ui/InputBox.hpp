#pragma once
#include"TextBlock.h"


class InputBox : public TextBlock {
public:
	InputBox(sf::Font* font = nullptr, sf::Vector2f pos = sf::Vector2f(0, 0), sf::Vector2f size = sf::Vector2f(10, 10), std::string text = "", float charectSize = 36, sf::Color fillColor = sf::Color::White, std::string name = ""):TextBlock(font,pos,size,text,charectSize,fillColor,name) {

	}

	void HandleEvent(sf::Event e) override{
		sf::String str;
		if (e.type == sf::Event::TextEntered) {
			if (e.text.unicode == 8) {
				std::string str = Str();
				if (str.size() > 0) {
					str.pop_back();
				}
				Str(str);
			}
			else if (e.text.unicode < 128) {
				Str(Str() + (char)e.text.unicode);
			}
			
		}
	}

	void BackGroundSprite(sf::Texture* tx) {
		m_texture = *tx;
		m_backGround = sf::Sprite(*tx);
	}
	void BackGroundSprite(std::string str) {
		m_texture.loadFromFile(str);
		m_backGround=sf::Sprite(m_texture);
	}

	void Draw(sf::RenderTexture* tx) override {
		m_backGround.setPosition(m_transform.GlobalPosition());
		tx->draw(m_backGround);
		TextBlock::Draw(tx);
	}

private:
	sf::Sprite m_backGround;
	sf::Texture m_texture;
};