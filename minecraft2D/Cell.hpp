#pragma once
#include"Chunk.hpp"

class Cell;

struct Item {
	int id;
	std::string name;
	sf::Texture* m_atlas;
};

struct DragObject {
	int count;
	Cell* startCell;
	Item dragItem;
	sf::RectangleShape dragItemShape;
};

class Cell {
public:
	Cell() {

	}

	Cell(sf::Vector2f pos, sf::Vector2f size, sf::Font font, sf::Texture* backTexture) {
		m_pos = pos;
		m_size = size;
		m_itemSize = size * 0.7f;
		m_font = font;
		m_backTexture = backTexture;

		m_backR = sf::RectangleShape(m_size);
		m_itemR = sf::RectangleShape(m_itemSize);

		m_text = sf::Text(" ", m_font, 12);

		m_backR.setPosition(m_pos);
		m_backR.setTexture(m_backTexture);

		SetState(unFocus);

		SetCustomTextOffset(sf::Vector2f(m_size.x / 2, m_size.y));
		UpdateItem();
	}

	void SetCustomTextOffset(sf::Vector2f pos) {
		m_textOffset = pos;
	}

	void CopyFrom(Cell* cell) {
		m_item = cell->getItem();
		m_count = cell->getCount();
		UpdateItem();
	}

	bool PopItem(Item item, int count = 1) {
		if (m_count >= count && m_item.id == item.id) {
			m_count -= count;
			if (m_count == 0) {
				m_item.id = 0;
			}



			UpdateItem();
			return true;
		}
		return false;
	}
	bool AddItem(Item item, int count = 1) {
		if (m_item.id == 0) {
			m_item = item;
			m_count = count;
		}
		else if (m_item.id == item.id) {
			m_count += count;
		}
		else {
			return false;
		}

		UpdateItem();
		return true;

	}
	int getCount() {
		return m_count;
	}
	Item getItem() {
		return m_item;
	}

	void UpdateItem() {
		m_backR.setFillColor(m_color);

		if (getItem().id != 0) {
			m_itemR.setTexture(m_item.m_atlas);
			m_itemR.setTextureRect(sf::IntRect(Settings::textureResolution.x * (m_item.id % Settings::atlasSize.x) + 1, Settings::textureResolution.y * std::floor(m_item.id / Settings::atlasSize.y), Settings::textureResolution.x, Settings::textureResolution.y));
			m_itemR.setFillColor(m_color);
			m_itemR.setPosition(m_pos + (m_size - m_itemSize) * 0.5f);

			m_text.setString(std::to_string(m_count));
			m_text.setPosition(m_pos + m_textOffset);
		}
		else {
			m_itemR.setFillColor({ 0,0,0,0 });
			m_text.setString("");
		}

		OnItemChange.Invoke(this);
	}

	void Draw(sf::RenderTexture* renderSpace) {
		renderSpace->draw(m_backR);
		renderSpace->draw(m_itemR);
		renderSpace->draw(m_text);
	}



	bool IsPointEnter(sf::Vector2f pos) {
		return pos.x >= m_pos.x && pos.x <= m_pos.x + m_size.x &&
			pos.y >= m_pos.y && pos.y <= m_pos.y + m_size.y;
	}


	enum State {
		Focus,
		Select,
		unFocus
	};

	void SetState(State state) {
		m_state = state;

		//update color
		switch (m_state)
		{
		case Cell::Focus:
			m_color = m_focusColor;
			break;
		case Cell::Select:
			m_color = m_selectColor;
			break;
		case Cell::unFocus:
			m_color = m_standartColor;
			break;
		}

		UpdateItem();
	}

	Action<Cell*> OnItemChange;
private:
	sf::RectangleShape m_backR, m_itemR;
	sf::Text m_text;
	sf::Font m_font;

	sf::Vector2f m_textOffset;

	sf::Texture* m_backTexture;

	sf::Vector2f m_pos, m_size, m_itemSize;
	sf::Color m_color, m_focusColor = { 200,200,200,255 }, m_selectColor = { 100,100,100,255 }, m_standartColor = sf::Color::White;

	State m_state = unFocus;

	std::mutex m_mutex;

	Item m_item = { 0,"" };
	int m_count = 0;

	bool m_needUpdate = true;
};