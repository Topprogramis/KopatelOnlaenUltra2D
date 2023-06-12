#include"Inventory.h"
#include"TimeManage.h"
#include"World.h"


Inventory::Inventory(Transform transform, int ColumCount, std::string name) : UiElement(transform.GetPosition(), transform.GetSize(), name, false) {
	this->m_transform = transform;
	this->m_columCount = ColumCount;

	Init();
}

void Inventory::Init() {
	m_scrollArea.create(m_transform.GetSize().x, m_transform.GetSize().y);

	m_cellSize = this->m_transform.GetSize().x / m_columCount;

	m_borderSize = sf::Vector2f(m_cellSize / m_columCount, m_cellSize / m_columCount);

	m_cellSize = (this->m_transform.GetSize().x - m_borderSize.x * (m_columCount + 1)) / m_columCount;

	m_items = {};

	m_font.loadFromFile("font.ttf");

	m_backTx.loadFromFile("inventoryBack.png");
	m_back.setSize(m_transform.GetSize());
	m_back.setTexture(&m_backTx);

	OnOpen.AddListener(std::bind(&Inventory::Opened, this));
	OnClose.AddListener(std::bind(&Inventory::Closed, this));

	m_animator = new Animator(this);
	m_animator->AddAnimClip("OpenInventory", new Vec2AnimationClip("position", 1.f, 1.f, false, glm::vec2(Settings::windowSize.x / 2 - 300, 800), glm::vec2(Settings::windowSize.x / 2 - 300, 50)));
	m_animator->AddAnimClip("CloseInventory", new Vec2AnimationClip("position", 1.f, 1.f, false, glm::vec2(Settings::windowSize.x / 2 - 300, 50), glm::vec2(Settings::windowSize.x / 2 - 300, 800)));

	m_animator->AddActionForEmit("OnOpen", &OnOpen);
	m_animator->AddActionForEmit("OnClose", &OnClose);

	m_animator->AddScriptFormStr("Open", "Play(OpenInventory);WaitWhileEnd();Emit(OnOpen);Call(Stay);");
	m_animator->AddScriptFormStr("Close", "Play(CloseInventory);WaitWhileEnd();Emit(OnClose);Call(Stay);");

	m_animator->AddScriptFormStr("Stay", "");

	LoadItems();

	for (int i = 0; i < 32; i++) {
		//m_cells.push_back(Cell());
	}

	m_cells[0].AddItem(m_items[2]);

	Settings::world->OnBlockBreak.AddListener(std::bind(&Inventory::CollectBlock, this, std::placeholders::_1));
}

//events
void Inventory::OnEnter(PointEventData data) {

}
void Inventory::OnExit(PointEventData data) {

}
void Inventory::OnStartDrag(PointEventData data) {
	sf::Vector2f localPos = sf::Vector2f(sf::Mouse::getPosition(*Settings::window)) - UiElement::m_transform.ScreenPosition();

	Cell* cell = getCellByLocalPos(localPos);

	if (cell!=nullptr && cell->getItem().id != 0) {
		m_dragObject.dragItem = cell->getItem();
		cell->PopItem(cell->getItem());

		//m_dragObject.startCellId = ind;

		m_dragObject.dragItemShape = sf::RectangleShape(sf::Vector2f((float)m_cellSize, (float)m_cellSize));
		m_dragObject.dragItemShape.setTexture(&m_atlas);
		m_dragObject.dragItemShape.setTextureRect(sf::IntRect(Settings::textureResolution.x * (m_dragObject.dragItem.id % Settings::atlasSize.x), Settings::textureResolution.y * std::floor(m_dragObject.dragItem.id / Settings::atlasSize.y), Settings::textureResolution.x, Settings::textureResolution.y));
		m_dragObject.dragItemShape.setFillColor(sf::Color::White);
	}
}
void Inventory::OnDrag(PointEventData data) {
	if (m_dragObject.dragItem.id != 0) {
		m_dragObject.dragItemShape.setPosition(data.pos-sf::Vector2f((float)m_cellSize/2,(float)m_cellSize/2));
	}
}
void Inventory::OnEndDrag(PointEventData data) {
	sf::Vector2f localPos = sf::Vector2f(sf::Mouse::getPosition(*Settings::window)) - UiElement::m_transform.ScreenPosition();

	if (localPos.x >= m_borderSize.x && localPos.x <= m_transform.GetSize().x - m_borderSize.x &&
		localPos.y >= 0 && localPos.y <= m_transform.GetSize().y) {

		int indX = localPos.x / (m_cellSize + m_borderSize.x);
		int indY = ((-m_srollOffset) + localPos.y) / (m_cellSize + m_borderSize.y * 2);

		int ind = indX + m_columCount * indY;

		if (ind < m_cells.size()) {
			if (m_cells[ind].AddItem(m_dragObject.dragItem)) {
				m_dragObject.dragItem.id = 0;
				m_dragObject.dragItemShape.setFillColor({ 0,0,0,0 });
			}
			else {
				m_cells[m_dragObject.startCellId].AddItem(m_dragObject.dragItem);
				m_dragObject.dragItemShape.setFillColor({ 0,0,0,0 });
				m_dragObject.dragItem.id = 0;
			}

		}
	}
}

void Inventory::CollectBlock(BlockData* data) {
	for (auto& i : m_cells) {
		if (i.AddItem(m_items[data->id]))
			break;
	}
}
Item Inventory::getSelect() {
	Item i = m_cells[0].getItem();
	m_cells[0].PopItem(i);
	return i;
}

Cell* Inventory::getCellByLocalPos(sf::Vector2f pos) {
	for (auto& i : m_cells) {
		if (i.IsPointEnter(pos))
			return &i;
	}
}


void Inventory::Open() {
	if (m_active == false && m_state == closed) {
		m_animator->SelectScript("Open");
		SetActive(true);
		m_state = opening;
	}

}
void Inventory::Close() {
	if (m_state == opened) {
		m_animator->SelectScript("Close");
		m_state = closing;
	}
}


void Inventory::LoadItems() {
	m_atlas.loadFromFile("atlas.png");

	std::vector<std::string> m_words;
	std::fstream settingsF;
	settingsF.open("items.txt");

	std::string name = "";

	while (settingsF.peek() != '#') {
		char ch = settingsF.get();

		if (ch == '\n')
			continue;

		if (ch == ';') {
			m_words.push_back(name);
			name = "";
			continue;
		}

		name += ch;
	}

	for (int i = 0; i < 256; i++) {
		std::string name = "None";

		if (i < m_words.size())
			name = m_words[i];
		AddItem(Item{ i, name });
	}


}
void Inventory::AddItem(Item item) {
	m_items.push_back(item);
}

void Inventory::FixedUpdate() {

}
void Inventory::Draw(sf::RenderTexture* window) {

	m_scrollArea.clear(sf::Color(0, 0, 0, 0));
	sf::RectangleShape sp;
	sf::RectangleShape back;
	sf::Text text = sf::Text(" ", m_font, 12);

	for (int i = 0; i < m_cells.size();i++) {

		auto cell = m_cells[i];

		int cellX = i - (m_columCount * (i / m_columCount));
		int cellY = i / m_columCount;

		int selectId = cell.getItem().id;

		sp.setTextureRect(sf::IntRect(Settings::textureResolution.x * (selectId % Settings::atlasSize.x), Settings::textureResolution.y * std::floor(selectId / Settings::atlasSize.y), Settings::textureResolution.x, Settings::textureResolution.y));

		sp.setTexture(&m_atlas);

		if(selectId!=0)
			text.setString(std::to_string(cell.getCount()));
		else
			text.setString("");

		sp.setSize(sf::Vector2f(m_cellSize, m_cellSize));
		back.setSize(sf::Vector2f(m_cellSize+2, m_cellSize+2));

		auto itemPos = sf::Vector2f(cellX * m_cellSize + m_borderSize.x * cellX, cellY * m_cellSize + m_borderSize.y * 2 * cellY + m_srollOffset);

		sp.setPosition(itemPos);
		back.setPosition(itemPos);
		text.setPosition(itemPos + sf::Vector2f(m_cellSize/2, m_cellSize));

		sp.setFillColor(sf::Color::White);
		back.setFillColor({ 150,150,150,150 });

		m_scrollArea.draw(back);
		m_scrollArea.draw(sp);
		m_scrollArea.draw(text);
	}

	m_scrollArea.display();

	sf::RectangleShape items(m_transform.GetSize()- sf::Vector2f(0,m_borderSize.y*2));
	items.setTexture(&m_scrollArea.getTexture());

	auto xOffset = m_transform.GetSize().x - (m_cellSize * m_columCount + m_borderSize.x * (m_columCount-1));

	items.setPosition(UiElement::m_transform.ScreenPosition() + sf::Vector2f(xOffset/2, m_borderSize.y));

	m_back.setPosition(UiElement::m_transform.ScreenPosition());

	window->draw(m_back);
	window->draw(items);

	window->draw(m_dragObject.dragItemShape);
}