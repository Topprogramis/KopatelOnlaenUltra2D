#include "World.h"
#include"TimeManage.h"

class Program {
public:
	Program() {
		Init();
	}

	void Init() {
		LoadSettings();
		InitGui();
		

		Start();

		m_mainThread=std::thread(std::bind(&Program::Loop,this));
		m_fixedUpdateThread=std::thread(std::bind(&Program::FixedUpdate, this));

		m_mainThread.join();
		m_fixedUpdateThread.join();

		End();
	}

	void End() {
		m_world->Save();
	}

	void Start() {
		m_world = new World(&m_window);
	}

	void Loop() {
		CreateWindow();

		while (m_window.isOpen()) {

			sf::Event e;
			while (m_window.pollEvent(e)) {
				ProcessEvent(e);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				m_world->Move(sf::Vector2f(0, 1000)* m_time.DeltaTime());
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				m_world->Move(sf::Vector2f(0, -1000) * m_time.DeltaTime());
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				m_world->Move(sf::Vector2f(1000, 0) * m_time.DeltaTime());
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				m_world->Move(sf::Vector2f(-1000, 0) * m_time.DeltaTime());
			}

			m_window.clear();



			m_world->Draw();

			m_window.draw(m_selectBlock);

			m_window.display();

			m_time.Update();
		}

		m_world->Save();
	}

	void Update() {

	}
	void FixedUpdate() {
		float time = 0.0f;

		while (1) {
			std::this_thread::sleep_for(std::chrono::milliseconds(Settings::fixedUpdateTime));
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				m_world->biuldBlock((sf::Vector2f)sf::Mouse::getPosition(m_window), &World::blocks[0]);
			else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
				m_world->biuldBlock((sf::Vector2f)sf::Mouse::getPosition(m_window), &World::blocks[m_selectId]);
			m_world->FixedUpdate();
		}
	}

	void ProcessEvent(sf::Event e) {
		if (e.type == sf::Event::Closed) {
			m_window.close();
		}
		else if (e.type == sf::Event::MouseButtonPressed) {
			
		}
		else if (e.type == sf::Event::MouseWheelScrolled) {
			SelectNewBlock(e);
		}
	}

	void SelectNewBlock(sf::Event e) {
		if (m_selectId + e.mouseWheelScroll.delta < 0) {
			m_selectId = 255;
		}
		else if (m_selectId + e.mouseWheelScroll.delta > 255) {
			m_selectId = 0;
		}
		else {
			m_selectId += e.mouseWheelScroll.delta;
		}


		m_selectBlock.setTextureRect(sf::IntRect(Settings::textureResolution.x * (m_selectId % Settings::atlasSize.x), Settings::textureResolution.y * std::floor(m_selectId / Settings::atlasSize.y), Settings::textureResolution.x, Settings::textureResolution.y));
	}
	void InitGui() {
		m_selectBlock.setSize(sf::Vector2f(100, 100));

		m_atlas.loadFromFile("atlas.png");
		m_selectBlock.setTexture(&m_atlas);
	}
	void CreateWindow() {
		m_window.create(sf::VideoMode(m_settings["WindowWidth"], m_settings["WindowHeight"]), "minecraft2D");

		m_window.setFramerateLimit(60);
	}
	void LoadSettings() {
		std::fstream settingsF;
		settingsF.open("settings.txt");

		std::string name;
		std::string value;

		bool initName = false;
		while (settingsF.peek() != '#') {
			char ch = settingsF.get();
			if (ch == '=') {
				initName = true;
			}
			else if (ch == '\n') {
				continue;
			}
			else if (ch == ';') {
				m_settings[name] = std::stoi(value);
				initName = false;
				name = "";
				value = "";
			}
			else {
				if (initName) {
					value += ch;
				}
				else {
					name += ch;
				}
			}
		}

		Settings::worldSize = m_settings["WorldSize"];

		Settings::blockSize.x = m_settings["BlockSizeX"];
		Settings::blockSize.y = m_settings["BlockSizeY"];

		Settings::textureResolution.x = m_settings["TextureResolutionX"];
		Settings::textureResolution.y = m_settings["TextureResolutionY"];

		Settings::atlasSize.x = m_settings["AtlasSizeX"];
		Settings::atlasSize.y = m_settings["AtlasSizeY"];

		Settings::fixedUpdateTime = m_settings["FixedUpdateTime"];
	}


private:
	sf::RenderWindow m_window;
	Time m_time;

	std::map<std::string, int> m_settings;

	sf::RectangleShape m_selectBlock;
	sf::Texture m_atlas;

	int m_selectId = 1;

	bool m_leftClick = false, m_rightClick = false;


	World* m_world;

	std::thread m_mainThread, m_fixedUpdateThread;
};

int main() {
	Program();
}