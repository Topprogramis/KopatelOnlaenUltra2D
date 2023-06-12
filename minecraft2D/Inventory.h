#include"include.h"
#include"Transform.h"
#include"Chunk.hpp"
#include"ui/UiElement.hpp"


struct Item {
	int id;
	std::string name;
};

struct DragObject {
	int startCellId;
	Item dragItem;
	sf::RectangleShape dragItemShape;

};

class Cell {
public:
	Cell(sf::Vector2f pos, sf::Vector2f size) {
		m_pos = pos;
	}

	bool PopItem(Item item, int count = 1) {
		if (m_count >= count && item.id == item.id) {
			m_count -= count;
			if (m_count == 0) {
				m_item.id = 0;
			}

			return true;
		}
		return false;
	}
	bool AddItem(Item item) {
		if (m_item.id == 0) {
			m_item = item;
			m_count = 1;
		}
		else if (m_item.id == item.id) {
			m_count++;
		}
		else {
			return false;
		}
		return true;
			
	}
	int getCount() {
		return m_count;
	}
	Item getItem() {
		return m_item;
	}

	bool IsPointEnter(sf::Vector2f pos) {
		return pos.x >= m_pos.x && pos.x <= m_pos.x + m_size.x &&
			pos.y >= m_pos.y && pos.y <= m_pos.y + m_size.y;
	}

private:
	sf::Vector2f m_pos, m_size;
	sf::Color m_color;

	Item m_item = {0,""};
	int m_count = 0;

};

class Inventory : public UiElement, public IOnStartDrag, public IOnDrag, public IOnEndDrag, public IOnEnter, public IOnExit {
public:
	Inventory(Transform trnasform = Transform(), int ColumCount=1,std::string name = "inventory");

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

	Item getSelect();

	void Draw(sf::RenderTexture* window) override;

	void CollectBlock(BlockData* data);
private:
	Transform m_transform;

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

	sf::Texture m_backTx;
	int m_columCount;

	sf::RectangleShape m_back;
	sf::Font m_font;

	DragObject m_dragObject;

	sf::RenderTexture m_scrollArea;

	sf::Vector2f m_borderSize = {}, m_cellOffset = {};

	std::vector<Cell> m_cells;

	std::vector<Item> m_items;

	int m_srollOffset = 0;
	int m_cellSize;

	void Init();

	Cell* getCellByLocalPos(sf::Vector2f pos);

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