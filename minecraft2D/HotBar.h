#pragma once
#include"include.h"
#include"Transform.h"
#include"Cell.hpp"
#include"ui/UiElement.hpp"

class HotBar : public UiElement {
public:
	HotBar(Transform trnasform = Transform(), int CellSize = 40, int cellOffset = 5, sf::Vector2i cellsCount = { 8,8 }, std::string name = "hotBar");

	void AddBindCell(Cell* cell) {
		m_bindedCells.push_back(cell);
	}

	Item getSelect();


	void HandleGlobalEvent(sf::Event e) override;

	void Draw(sf::RenderTexture* tx) override;
	void Init();


	void SetSelectSlot(int ind) {
		m_slots[m_selectInd]->SetState(Cell::unFocus);
		m_selectInd = ind;
		m_slots[m_selectInd]->SetState(Cell::Select);
	}

private:
	sf::RenderTexture m_slotsArea;
	sf::RectangleShape m_back;
	sf::Font m_font;

	std::vector<Cell*> m_slots = {};
	std::vector<Cell*> m_bindedCells = {};

	sf::Vector2f m_borderSize = {}, m_cellOffset = {};
	sf::Vector2i m_cellsCount;
	int m_cellSize;

	int m_selectInd = 0;

	sf::Texture m_backTx, m_cellBackTx;

};