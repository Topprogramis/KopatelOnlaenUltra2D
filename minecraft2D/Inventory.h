#pragma once
#include"include.h"
#include"Cell.hpp"
#include"ui/UiElement.hpp"
#include"HotBar.h"

class Inventory : public UiElement, public IOnStartDrag, public IOnDrag, public IOnEndDrag, public IOnEnter, public IOnExit {
public:
	Inventory(Transform trnasform = Transform(), int CellSize = 40, int cellOffset = 5, sf::Vector2i cellsCount = {8,8}, std::string name = "inventory");

	void BindHotBar(HotBar* hotBar);

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

	HotBar* m_HotBar;

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

	Cell* getCellByLocalPos(sf::Vector2f pos, sf::Vector2f size);

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