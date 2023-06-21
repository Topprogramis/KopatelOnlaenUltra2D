#include"Inventory.h"
#include"TimeManage.h"
#include"World.h"

Inventory::Inventory(Transform transform, int cellSize, int cellOffset, sf::Vector2i cellsCount, std::string name) : UiElement(transform.GetPosition(), transform.GetSize(), name, false) {
	m_cellSize = cellSize;
	m_cellsCount = cellsCount;
	m_cellOffset = sf::Vector2f(cellOffset,cellOffset);
	m_borderSize = { 15,15 };

	Init();
}

void Inventory::BindHotBar(HotBar* hotBar) {
	m_HotBar = hotBar;

	for (int x = 0; x < m_cellsCount.x; x++) {
		m_HotBar->AddBindCell(m_cells[x]);
	}
	hotBar->Init();
}

void Inventory::Init() {
	m_scrollArea.create(m_transform.Size().x, m_transform.Size().y);

	m_items = {};

	m_font.loadFromFile("font.ttf");

	m_cellBackTx.loadFromFile("inventoryCell.png");
	m_backTx.loadFromFile("inventoryBack.png");
	m_back.setSize(m_transform.Size());
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

	m_dragObject.dragItemShape = sf::RectangleShape(sf::Vector2f((float)m_cellSize, (float)m_cellSize));
	m_dragObject.dragItemShape.setTexture(&m_atlas);

	m_dragObject.dragItemShape.setFillColor({ 0,0,0,0 });
	m_dragObject.dragItem.id = 0;
	
	CreatCells();
	m_cells[0]->AddItem(m_items[2]);
	m_cells[1]->AddItem(m_items[5]);

	Settings::world->OnBlockBreak.AddListener(std::bind(&Inventory::CollectBlock, this, std::placeholders::_1));

}

void Inventory::Load() {
	std::ifstream saveFile;

	saveFile.open("saves\\player.txt");
	if (!saveFile.is_open()) {
		std::cout << "Save is not found. Please create in game files saves/player.txt" << std::endl;
		return;
	}

	std::string id, count, buf;
	bool initId = false;

	std::getline(saveFile, buf);
	std::getline(saveFile, buf);
	for (auto& i : buf) {
		if (i == ',') {
			initId = true;
			continue;
		}
		else if (i == ';') {
			if (count != "0") {
				AddItem(std::stoi(id), std::stoi(count));

				initId = false;
				id = "";
				count = "";
			}
		}
		else if (initId) {
			count += i;
		}
		else {
			id += i;
		}
	}

	saveFile.close();
}

void Inventory::CreatCells() {
	for (int y = 0; y < m_cellsCount.y; y++) {
		for (int x = 0; x < m_cellsCount.x; x++) {
			m_cells.push_back(new Cell(sf::Vector2f(x * (float)m_cellSize + x*m_cellOffset.x, y * (float)m_cellSize + y * m_cellOffset.y), sf::Vector2f(m_cellSize, m_cellSize), m_font, &m_cellBackTx));
		}
	}
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
		std::unique_lock<std::recursive_mutex> lock(m_mutex);
		m_dragObject.dragItem = cell->getItem();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
			m_dragObject.count = cell->getCount();
		else
			m_dragObject.count = 1;


		cell->PopItem(cell->getItem(),m_dragObject.count);

		lock.unlock();

		m_dragObject.startCell = cell;

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

	Cell* cell = getCellByLocalPos(localPos);

	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (cell != nullptr && cell->AddItem(m_dragObject.dragItem, m_dragObject.count)) {
		m_dragObject.dragItem.id = 0;
		m_dragObject.dragItemShape.setFillColor({ 0,0,0,0 });
	}
	else {
		if(m_dragObject.startCell!=nullptr)
			m_dragObject.startCell->AddItem(m_dragObject.dragItem, m_dragObject.count);
		m_dragObject.dragItemShape.setFillColor({ 0,0,0,0 });
		m_dragObject.dragItem.id = 0;
	}
	lock.unlock();

}

void Inventory::PopItem(int id, int count) {
	for (auto& i : m_cells) {
		if (i->PopItem(m_items[id],count))
			break;
	}
}
void Inventory::CollectBlock(BlockData* data) {
	for (auto& i : m_cells) {
		if (i->AddItem(m_items[data->id]))
			break;
	}
}
Item Inventory::getSelect() {
	return m_HotBar->getSelect();
}

Cell* Inventory::getCellByLocalPos(sf::Vector2f pos) {
	for (auto& i : m_cells) {
		if (i->IsPointEnter(pos))
			return i;
	}
	return nullptr;
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

std::string Inventory::Serialize() {
	std::string data;

	for (auto& i : m_cells) {
		data += std::to_string(i->getItem().id) + "," + std::to_string(i->getCount()) + ";";
	}

	return data;
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
		AddItem(Item{ i, name,&m_atlas });
	}


}
void Inventory::AddItem(int id, int count) {
	for (auto& i : m_cells) {
		if (i->AddItem(m_items[id], count))
			break;
	}
}
void Inventory::AddItem(Item item) {
	m_items.push_back(item);
}

void Inventory::FixedUpdate() {

}
void Inventory::Draw(sf::RenderTexture* window) {

	m_scrollArea.clear(sf::Color(0, 0, 0, 0));

	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	for (auto& i : m_cells) {
		i->Draw(&m_scrollArea);
	}
	lock.unlock();

	m_scrollArea.display();



	sf::RectangleShape items(m_transform.Size());
	items.setTexture(&m_scrollArea.getTexture());

	items.setPosition(UiElement::m_transform.ScreenPosition() + m_borderSize);

	m_back.setPosition(UiElement::m_transform.ScreenPosition());

	window->draw(m_back);
	window->draw(items);

	window->draw(m_dragObject.dragItemShape);
}