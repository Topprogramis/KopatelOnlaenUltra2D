#pragma once
#include"include.h"
#include"Transform.h"
#include"Chunk.hpp"
#include"ui/UiElement.hpp"

class Cell;

struct Item {
	int id;
	std::string name;
	sf::Texture* m_atlas;
};

struct DragObject {
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
			m_count+= count;
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
			m_itemR.setTextureRect(sf::IntRect(Settings::textureResolution.x * (m_item.id % Settings::atlasSize.x)+1, Settings::textureResolution.y * std::floor(m_item.id / Settings::atlasSize.y), Settings::textureResolution.x, Settings::textureResolution.y));
			m_itemR.setFillColor(m_color);
			m_itemR.setPosition(m_pos + (m_size- m_itemSize) *0.5f);

			m_text.setString(std::to_string(m_count));
			m_text.setPosition(m_pos + sf::Vector2f(m_size.x / 2, m_size.y));
		}
		else {
			m_itemR.setFillColor({ 0,0,0,0 });
			m_text.setString("");
		}
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
	}


private:
	sf::RectangleShape m_backR, m_itemR;
	sf::Text m_text;
	sf::Font m_font;

	sf::Texture* m_backTexture;

	sf::Vector2f m_pos, m_size, m_itemSize;
	sf::Color m_color, m_focusColor = { 200,200,200,255 }, m_selectColor = {100,100,100,255}, m_standartColor = sf::Color::White;

	State m_state = unFocus;

	std::mutex m_mutex;

	Item m_item = {0,""};
	int m_count = 0;

	bool m_needUpdate = true;
};

class Inventory : public UiElement, public IOnStartDrag, public IOnDrag, public IOnEndDrag, public IOnEnter, public IOnExit {
public:
	Inventory(Transform trnasform = Transform(), int CellSize = 40, int cellOffset = 5, sf::Vector2i cellsCount = {8,8}, std::string name = "inventory");

	//events
	void OnStartDrag(PointEventData data) override;
	void OnDrag(PointEventData data) override;
	void OnEndDrag(PointEventData data) override;
	void OnExit(PointEventData data) override;
	void OnEnter(PointEventData data) override;

	void FixedUpdate() override;

	bool isOpen() { return m_state == opened; }

	void Close();
	void Open();

	void Load();

	Item getSelect();
	std::string Serialize();


	void Draw(sf::RenderTexture* window) override;

	void AddItem(int id, int count = 1);
	void PopItem(int id, int count = 1);

	void CollectBlock(BlockData* data);
private:
	Animator* m_animator;
	sf::Texture m_atlas;

	Item* m_focus = nullptr, *m_select = nullptr;

	enum AnimState {
		opened,
		closed,
		closing,
		opening
	};
	Action<> OnOpen, OnClose;
	AnimState m_state = closed;

	sf::Texture m_backTx, m_cellBackTx;

	sf::RectangleShape m_back;
	sf::Font m_font;

	DragObject m_dragObject;

	sf::RenderTexture m_scrollArea;

	sf::Vector2f m_borderSize = {}, m_cellOffset = {};
	sf::Vector2i m_cellsCount;
	int m_cellSize;

	std::vector<Cell*> m_cells;
	std::vector<Item> m_items;

	std::recursive_mutex m_mutex;

	void Init();

	Cell* getCellByLocalPos(sf::Vector2f pos);

	void CreatCells();
	void LoadItems();
	void AddItem(Item item);

	void Opened() {
		m_state = opened;
	}
	void Closed() {
		SetActive(false);
		m_state = closed;
	}
};