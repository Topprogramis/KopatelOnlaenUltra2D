#include"Inventory.h"
#include"TimeManage.h"


Inventory::Inventory(Transform transform, int ColumCount) {
	this->m_transform = transform;
	this->m_columCount = ColumCount;

	Init();
}

void Inventory::Create(Transform transform, int ColumCount) {
	this->m_transform = transform;
	this->m_columCount = ColumCount;

	Init();
}


void Inventory::Init() {
	m_scrollArea.create(m_transform.GetSize().x, m_transform.GetSize().y);

	m_cellSize = this->m_transform.GetSize().x / m_columCount;

	m_borderSize = sf::Vector2f( m_cellSize / m_columCount, m_cellSize / m_columCount);

	m_cellSize = (this->m_transform.GetSize().x - m_borderSize.x * (m_columCount + 1)) / m_columCount;

	m_items = {};

	m_font.loadFromFile("font.ttf");

	m_backTx.loadFromFile("inventoryBack.png");
	m_back.setSize(m_transform.GetSize());
	m_back.setTexture(&m_backTx);
}

Item Inventory::getSelect() {
	if (m_select == nullptr)
		return m_items[0];

	return *m_select;
}

void Inventory::AddItem(Item item) {
	m_items.push_back(item);
}

void Inventory::PollEvent(sf::Event e, sf::RenderWindow* window) {
	if (OnEnter(sf::Mouse::getPosition(*window))) {
		if (e.type == sf::Event::MouseWheelScrolled) {
			m_srollOffset += e.mouseWheelScroll.delta * Time::globalTimeManager->DeltaTime() * 2000;
		}
		
		sf::Vector2f localPos = sf::Vector2f(sf::Mouse::getPosition(*window)) - m_transform.GetPosition();

		if (localPos.x  >= m_borderSize.x && localPos.x <= m_transform.GetSize().x- m_borderSize.x &&
			localPos.y >= 0 && localPos.y <= m_transform.GetSize().y) {

			int indX = localPos.x / (m_cellSize+m_borderSize.x) ;
			int indY = ((-m_srollOffset)+localPos.y)/(m_cellSize + m_borderSize.y*2);

			int ind = indX + m_columCount * indY;

			if (ind < m_items.size()) {
				if (m_focus != nullptr)
					m_focus->m_cl = sf::Color::White;


				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					if (m_select != nullptr && m_select != &m_items[ind])
						m_select->m_cl = sf::Color::White;

					m_select = &m_items[ind];
					m_select->m_cl = sf::Color(150, 150, 150);

					m_focus = nullptr;
				}
				else if(&m_items[ind]!= m_select) {
					m_focus = &m_items[ind];
					m_focus->m_cl = sf::Color(200, 200, 200);
				}
			}
		}
	}

	if (e.type == sf::Event::KeyPressed) {
		if (e.key.code == sf::Keyboard::E) {

			if (m_state == opened) {
				m_state = closing;
			}
			else if (m_state == closed) {
				m_state = opening;
			}
		}
	}
}

void Inventory::FixedUpdate() {
	
}

void Inventory::Draw(sf::RenderWindow* window) {
	if (m_state == opening) {
		if (m_transform.GetPosition().y <= 100)
			m_state = opened;
		else
			m_transform.Move(sf::Vector2f(0, -3000) * Time::globalTimeManager->DeltaTime());
	}
	else if (m_state == closing) {
		if (m_transform.GetPosition().y >= 800)
			m_state = closed;
		else
			m_transform.Move(sf::Vector2f(0, 3000)*Time::globalTimeManager->DeltaTime());
	}

	if (m_state == closed)
		return;

	m_scrollArea.clear(sf::Color(0, 0, 0, 0));
	sf::RectangleShape sp;
	sf::Text text = sf::Text(" ", m_font, 12);

	for (int i = 0; i < m_items.size();i++) {

		auto cell = m_items[i];

		int cellX = i - (m_columCount * (i / m_columCount));
		int cellY = i / m_columCount;

		int selectId = cell.id;

		sp.setTextureRect(sf::IntRect(Settings::textureResolution.x * (selectId % Settings::atlasSize.x), Settings::textureResolution.y * std::floor(selectId / Settings::atlasSize.y), Settings::textureResolution.x, Settings::textureResolution.y));

		sp.setTexture(cell.atlas);
		text.setString(cell.name);

		sp.setSize(sf::Vector2f(m_cellSize, m_cellSize));

		auto itemPos = sf::Vector2f(cellX * m_cellSize + m_borderSize.x * cellX, cellY * m_cellSize + m_borderSize.y * 2 * cellY + m_srollOffset);

		sp.setPosition(itemPos);
		text.setPosition(itemPos + sf::Vector2f(5, m_cellSize));

		sp.setFillColor(cell.m_cl);

		m_scrollArea.draw(sp);
		m_scrollArea.draw(text);
	}

	m_scrollArea.display();

	sf::RectangleShape items(m_transform.GetSize()- sf::Vector2f(0,m_borderSize.y*2));
	items.setTexture(&m_scrollArea.getTexture());

	auto xOffset = m_transform.GetSize().x - (m_cellSize * m_columCount + m_borderSize.x * (m_columCount-1));

	items.setPosition(m_transform.GetPosition() + sf::Vector2f(xOffset/2, m_borderSize.y));

	m_back.setPosition(m_transform.GetPosition());

	window->draw(m_back);
	window->draw(items);
}

bool Inventory::OnEnter(sf::Vector2i pos) {
	return pos.x >= m_transform.GetPosition().x && pos.x <= m_transform.GetPosition().x + m_transform.GetSize().x &&
		pos.y >= m_transform.GetPosition().y && pos.y <= m_transform.GetPosition().y + m_transform.GetSize().y;
}