#include"HotBar.h"

HotBar::HotBar(Transform transform, int cellSize, int cellOffset, sf::Vector2i cellsCount, std::string name) : UiElement(transform.GetPosition(), transform.GetSize(), name, true) {
	m_cellSize = cellSize;
	m_cellsCount = cellsCount;
	m_cellOffset = sf::Vector2f(cellOffset, cellOffset);
	m_borderSize = { (float)cellOffset, (float)cellOffset };
}

Item HotBar::getSelect() {
	Item i = m_bindedCells[m_selectInd]->getItem();
	return i;
}


void HotBar::HandleGlobalEvent(sf::Event e) {
	if (e.type == sf::Event::KeyPressed) {
		switch(e.key.code) {
		case sf::Keyboard::Num1:
			SetSelectSlot(0);
			break;
		case sf::Keyboard::Num2:
			SetSelectSlot(1);
			break;
		case sf::Keyboard::Num3:
			SetSelectSlot(2);
			break;
		case sf::Keyboard::Num4:
			SetSelectSlot(3);
			break;
		case sf::Keyboard::Num5:
			SetSelectSlot(4);
			break;
		case sf::Keyboard::Num6:
			SetSelectSlot(5);
			break;
		case sf::Keyboard::Num7:
			SetSelectSlot(6);
			break;
		case sf::Keyboard::Num8:
			SetSelectSlot(7);
			break;
		case sf::Keyboard::Num9:
			SetSelectSlot(8);
			break;
		}
	}
}

void HotBar::Init() {
	m_slotsArea.create(m_transform.Size().x, m_transform.Size().y);
	m_back.setSize(m_transform.Size());

	m_font.loadFromFile("font.ttf");

	m_backTx.loadFromFile("HotBar.png");
	m_cellBackTx.loadFromFile("inventoryCell.png");

	m_back.setTexture(&m_backTx);

	for (int x = 0; x < m_cellsCount.x; x++) {
		m_slots.push_back(new Cell(sf::Vector2f(x * (float)m_cellSize + x * m_cellOffset.x,0), sf::Vector2f(m_cellSize, m_cellSize), m_font, &m_cellBackTx));
		m_slots[x]->SetCustomTextOffset(sf::Vector2f(0, m_cellSize -12));
		m_bindedCells[x]->OnItemChange.AddListener(std::bind(&Cell::CopyFrom, m_slots[x], std::placeholders::_1));
	}

	SetSelectSlot(0);
}

void HotBar::Draw(sf::RenderTexture* window) {
	m_slotsArea.clear(sf::Color(0, 0, 0, 0));

	for (auto& i : m_slots)
		i->Draw(&m_slotsArea);

	m_slotsArea.display();

	sf::RectangleShape items(m_transform.Size());
	items.setTexture(&m_slotsArea.getTexture());
	items.setPosition(UiElement::m_transform.ScreenPosition() + m_borderSize);

	m_back.setPosition(UiElement::m_transform.ScreenPosition());

	window->draw(m_back);
	window->draw(items);
}