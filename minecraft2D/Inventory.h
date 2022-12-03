#include"include.h"
#include"Transform.h"

struct Item {
	sf::Texture* atlas;
	int id;
	std::string name;

	sf::Color m_cl = sf::Color::White;
};

class Inventory {
public:
	Inventory(Transform trnasform = Transform(), int ColumCount=1);

	void Create(Transform trnasform = Transform(), int ColumCount = 1);

	void PollEvent(sf::Event e, sf::RenderWindow* window);

	void FixedUpdate();

	bool isOpen() { return m_state == opened; }
	void Close();

	Item getSelect();

	void Draw(sf::RenderWindow* window);

	void AddItem(Item item);

	bool OnEnter(sf::Vector2i);

private:
	Transform m_transform;

	Item* m_focus = nullptr, *m_select = nullptr;

	enum AnimState {
		opened,
		opening,
		closing,
		closed
	};

	AnimState m_state = opened;


	sf::Texture m_backTx;
	int m_columCount;

	sf::RectangleShape m_back;
	sf::Font m_font;

	sf::RenderTexture m_scrollArea;

	sf::Vector2f m_borderSize = {}, m_cellOffset = {};

	std::vector<Item> m_items;

	int m_srollOffset = 0;
	int m_cellSize;

	void Init();

	void LoadItems();


};