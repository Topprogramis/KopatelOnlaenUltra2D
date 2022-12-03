#include "World.h"
#include"TimeManage.h"
#include"Inventory.h"

class Program {
public:
	Program() {
		Init();
	}

	void Init() {
		LoadSettings();
		InitGui();
		

		Start();

		Time::globalTimeManager = &m_time;

		m_mainThread=std::thread(std::bind(&Program::Loop,this));
		m_fixedUpdateThread=std::thread(std::bind(&Program::FixedUpdate, this));
		m_buildChunkThread = std::thread(std::bind(&Program::ChunkBlocksUpdate, this));

		m_mainThread.join();
		m_fixedUpdateThread.join();
		m_buildChunkThread.join();

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

		while (m_run || m_fixedRun || m_chunkRun) {

			sf::Event e;
			while (m_window.pollEvent(e)) {
				ProcessEvent(e);
				m_inventory.PollEvent(e,&m_window);
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

			m_inventory.Draw(&m_window);


			m_window.display();

			m_time.Update();

		}

		

	//	m_world->Save();

		m_window.close();

		//d::terminate();
	}

	void Update() {

	}
	void FixedUpdate() {
		float time = 0.0f;

		bool IsDown = false;

		while (m_run) {
			std::this_thread::sleep_for(std::chrono::milliseconds(Settings::fixedUpdateTime));
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !m_inventory.isOpen())
				m_world->DestroyBlock((sf::Vector2f)sf::Mouse::getPosition(m_window));
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !m_inventory.isOpen()) {
				if (!m_world->buildBlock((sf::Vector2f)sf::Mouse::getPosition(m_window), &World::blocks[m_inventory.getSelect().id]) && !IsDown) {
					m_world->Interact();
				}
				IsDown = true;
			}
			else if (!sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
				IsDown = false;
			}
			m_world->FixedUpdate();

			m_inventory.FixedUpdate();

			if (m_interact) {
				//m_world->Interact();
				m_interact = false;
			}
		}

		m_fixedRun = false;

		std::cout << "FixedUpdate end" << std::endl;
	}

	void ChunkBlocksUpdate() {
		float time = 0.0f;

		while (m_run) {		
			std::this_thread::sleep_for(std::chrono::milliseconds(Settings::fixedUpdateTime));
			m_world->UpdateBlocks();
		}

		m_chunkRun = false;

		std::cout << "chunk update end" << std::endl;
	}

	void ProcessEvent(sf::Event e) {
		if (e.type == sf::Event::Closed) {
			m_run = false;
		}
	}

	void SelectNewBlock(sf::Event e) {
	}
	void LoadItems() {
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

		
	}
	void InitGui() {

		m_atlas.loadFromFile("atlas.png");

		LoadItems();

		m_inventory.Create(Transform(sf::Vector2f(200, 100), sf::Vector2f(600, 600)),8);

		for (int i = 0; i < 256; i++) {
			std::string name = "None";

			if (i < m_words.size())
				name = m_words[i];
			m_inventory.AddItem(Item{ &m_atlas,i, name });
		}
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

		std::string seed;


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

		Settings::persistence = m_settings["Persistence"];
		Settings::frequency = m_settings["Frequency"];
		Settings::amplitude = m_settings["Amplitude"];
		Settings::octaves = m_settings["Octaves"];

		Settings::seed = m_settings["Seed"];

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

	Inventory m_inventory;

	std::map<std::string, int> m_settings;

	sf::Texture m_atlas;

	bool m_interact = false;

	bool m_run = true;

	bool m_fixedRun = true, m_chunkRun = true;

	std::mutex m_mutex;

	std::vector<std::string> m_words;


	World* m_world;

	std::thread m_mainThread, m_fixedUpdateThread,m_buildChunkThread;
};

int main() {
	Program();
}