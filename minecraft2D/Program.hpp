#pragma once
#include "World.h"
#include"TimeManage.h"
#include"ui/GuiManager.hpp"
#include"BlockController.h"
#include"ui/Animator.hpp"

class Program {
public:
	Program() {
		Init();
	}

	void Init() {
		LoadSettings();

		Start();

		Time::globalTimeManager = &m_time;

		m_mainThread = std::thread(std::bind(&Program::Loop, this));
		m_uiMainThread = std::thread(std::bind(&Program::GuiLoop, this));
		m_fixedUpdateThread = std::thread(std::bind(&Program::FixedUpdate, this));
		m_buildChunkThread = std::thread(std::bind(&Program::ChunkBlocksUpdate, this));
		m_physicThread = std::thread(std::bind(&Program::PhysicUpdate, this));
		m_animatonThread = std::thread(std::bind(&Program::AnimationUpdate, this));
		//m_eventProcessThread = std::thread(std::bind(&Program::EventThread, this));

		m_mainThread.join();

		m_uiMainThread.join();
		m_fixedUpdateThread.join();
		m_buildChunkThread.join();
		m_physicThread.join();
		m_animatonThread.join();
		//m_eventProcessThread.join();

		End();
	}

	void LoadCollisionList() {
		std::cout << "loading collisions..." << std::endl;

		std::string list = m_settings["CollisionList"];
		std::vector<std::string> collisions = {};

		std::string currentName;
		for (auto& i : list) {
			if (i == '{')
				continue;
			else if (i == ',' || i == '}') {
				collisions.push_back(currentName);
				currentName = "";
			}
			else {
				currentName += i;
			}
		}

		for (auto& col : collisions) {
			Settings::AddCol(col);
		}

		std::cout << "Collision load successful" << std::endl;
	}

	void End() {
		m_world->Save();
	}

	void Start() {
		m_world = new World(&m_window);
		Settings::window = &m_window;
		Settings::program = this;
	}

	void StartAfterInit() {

	}


	void InitThreads() {
		//animation thread
		m_animationControlConv.notify_one();

		std::mutex mAnimation;
		std::unique_lock<std::mutex> lockAnimation(mAnimation);
		m_animationConv.wait(lockAnimation);
		lockAnimation.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		//gui thread
		m_guiControlConv.notify_one();

		std::mutex mGui;
		std::unique_lock<std::mutex> lock(mGui);
		m_guiConv.wait(lock);
		lock.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		//physic
		m_physicControlConv.notify_one();

		std::mutex mPhysic;
		std::unique_lock<std::mutex> lock2(mPhysic);
		m_physicConv.wait(lock2);
		lock2.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		//fixed update
		m_fixedControlConv.notify_one();

		std::mutex mFixed;
		std::unique_lock<std::mutex> lock3(mFixed);
		m_fixedConv.wait(lock3);
		lock3.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		//block update

		m_blockControlUpdate.notify_one();

		std::mutex mBlock;
		std::unique_lock<std::mutex> lock4(mBlock);
		m_blockUpdate.wait(lock4);
		lock4.unlock();


	}

	void GuiLoop() {
		std::mutex m;
		std::unique_lock<std::mutex> lock(m);
		m_guiControlConv.wait(lock);
		lock.unlock();

		InitGui();

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		m_guiConv.notify_one();

		std::cout << "GUI thread init successful" << std::endl;

		while (m_run)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(15));
			m_GUImanager->Update();
			m_GUImanager->DrawUiElements();
		}

		std::cout << "GUI thread end" << std::endl;
	}

	void Loop() {
		CreateWindow();

		std::cout << "Main thread init successful" << std::endl;

		InitThreads();

		StartAfterInit();


		while (m_run || m_fixedRun || m_chunkRun || m_physicRun) {
			sf::Event e;
			while (m_window.pollEvent(e)) {
				ProcessEvent(e);
				m_GUImanager->HandleEvnet(e);
			}


			m_window.clear();

			m_world->Draw();
			m_blockController->Draw();
			m_GUImanager->Draw();

			m_window.display();

			m_time.Update();

		}

		std::cout << "Main thread end" << std::endl;



		//	m_world->Save();

		m_window.close();

		//d::terminate();
	}

	void Update() {

	}

	void AnimationUpdate() {
		std::mutex m;
		std::unique_lock<std::mutex> lock(m);
		m_animationControlConv.wait(lock);
		lock.unlock();

		float time = 0.0f;

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		m_animationConv.notify_one();

		std::cout << "Animation update thread init successful" << std::endl;

		while (m_fixedRun) {
			std::this_thread::sleep_for(std::chrono::milliseconds(Settings::fixedUpdateTime));

			for (auto& i : m_animators)
				i->Process();
		}

		std::cout << "Animation update thread end" << std::endl;
	}
	void EventThread() {
		std::mutex m;
		std::unique_lock<std::mutex> lock(m);
		m_eventControlConv.wait(lock);
		lock.unlock();

		float time = 0.0f;

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		m_eventConv.notify_one();

		std::cout << "Event thread init successful" << std::endl;

		while (m_run) {
			std::unique_lock<std::recursive_mutex> lock(m_eventMutex);
			std::vector<sf::Event> poll = m_poll;
			m_poll.clear();
			lock.unlock();
			for (auto& e : poll) {
				ProcessEvent(e);
				m_GUImanager->HandleEvnet(e);
			}
		}

		std::cout << "Event thread end" << std::endl;
	}
	void FixedUpdate() {
		std::mutex m;
		std::unique_lock<std::mutex> lock(m);
		m_fixedControlConv.wait(lock);
		lock.unlock();

		float time = 0.0f;

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		m_fixedConv.notify_one();

		std::cout << "Fixed update thread init successful" << std::endl;

		while (m_fixedRun) {
			std::this_thread::sleep_for(std::chrono::milliseconds(Settings::fixedUpdateTime));

			m_world->FixedUpdate();
			m_blockController->FixedUpdate();
			m_GUImanager->FixedUpdate();

			if (m_interact) {
				//m_world->Interact();
				m_interact = false;
			}
		}

		std::cout << "Fixed update thread end" << std::endl;
	}

	void ChunkBlocksUpdate() {
		std::mutex m;
		std::unique_lock<std::mutex> lock(m);
		m_blockControlUpdate.wait(lock);
		lock.unlock();

		float time = 0.0f;

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		m_blockUpdate.notify_one();

		std::cout << "Chunk update thread init successful" << std::endl;

		while (m_chunkRun) {
			std::this_thread::sleep_for(std::chrono::milliseconds(Settings::fixedUpdateTime));
			m_world->UpdateBlocks();
		}

		std::cout << "chunk update end" << std::endl;
	}

	void PhysicUpdate() {
		std::mutex m;
		std::unique_lock<std::mutex> lock(m);
		m_physicControlConv.wait(lock);
		lock.unlock();

		float time = 0.0f;

		m_physicConv.notify_one();

		std::cout << "physic thread init successful" << std::endl;

		while (m_physicRun) {
			std::this_thread::sleep_for(std::chrono::milliseconds(Settings::physicUpdateTime));
			m_world->PhysicUpdate();
		}

		std::cout << "physic update end" << std::endl;
	}

	void ProcessEvent(sf::Event e) {
		if (e.type == sf::Event::Closed) {
			m_run = false;

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			m_physicRun = false;
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			m_chunkRun = false;
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			m_fixedRun = false;
		}
		else if (e.type == sf::Event::KeyPressed) {
			if (e.key.code == sf::Keyboard::E) {

				if (m_inventory->GetActive() == true)
					m_inventory->Close();
				else
					m_inventory->Open();
			}
		}

	}

	void SelectNewBlock(sf::Event e) {
	}
	void LoadItems() {
		
	}
	void InitGui() {


		m_GUImanager = new GuiManager(sf::Vector2f(0, 0), Settings::windowSize, { 0,0,0,0 });
		m_GUImanager->setWindow(&m_window);

		m_GUImanager->AddElemnt(new Inventory(Transform(sf::Vector2f(200, 100), sf::Vector2f(600, 600)), 8, "PlayerInventory"));

		m_inventory = static_cast<Inventory*>(m_GUImanager->getElementByName("PlayerInventory"));

		m_blockController = new BlockController(m_inventory);
	}
	void CreateWindow() {
		m_window.create(sf::VideoMode(Settings::windowSize.x, Settings::windowSize.y), "minecraft2D");

		m_window.setFramerateLimit(60);
	}
	void LoadSettings() {
		std::cout << "settings loading..." << std::endl;

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
				m_settings[name] = value;

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

		Settings::worldSize = std::stoi(m_settings["WorldSize"]);

		Settings::persistence = std::stoi(m_settings["Persistence"]);
		Settings::frequency = std::stoi(m_settings["Frequency"]);
		Settings::amplitude = std::stoi(m_settings["Amplitude"]);
		Settings::octaves = std::stoi(m_settings["Octaves"]);

		Settings::seed = std::stoi(m_settings["Seed"]);

		Settings::blockSize.x = std::stoi(m_settings["BlockSizeX"]);
		Settings::blockSize.y = std::stoi(m_settings["BlockSizeY"]);

		Settings::textureResolution.x = std::stoi(m_settings["TextureResolutionX"]);
		Settings::textureResolution.y = std::stoi(m_settings["TextureResolutionY"]);

		Settings::atlasSize.x = std::stoi(m_settings["AtlasSizeX"]);
		Settings::atlasSize.y = std::stoi(m_settings["AtlasSizeY"]);

		Settings::windowSize.x = std::stoi(m_settings["WindowWidth"]);
		Settings::windowSize.y = std::stoi(m_settings["WindowHeight"]);

		Settings::fixedUpdateTime = std::stoi(m_settings["FixedUpdateTime"]);
		Settings::physicUpdateTime = std::stoi(m_settings["PhysicUpdateTime"]);

		Settings::gravy = std::stof(m_settings["Gravy"]);

		std::cout << "settings load successful" << std::endl;

		LoadCollisionList();
	}

	void AddAnimatorObject(Animator* anim) {
		m_animators.push_back(anim);
	}


private:
	sf::RenderWindow m_window;
	Time m_time;

	Inventory* m_inventory;
	BlockController* m_blockController;
	GuiManager* m_GUImanager;

	std::vector<Animator*> m_animators;

	std::map<std::string, std::string> m_settings;

	sf::Texture m_atlas;

	bool m_interact = false;

	bool m_run = true;

	bool m_fixedRun = true, m_chunkRun = true, m_physicRun = true;

	std::condition_variable m_fixedConv, m_physicConv, m_blockUpdate, m_guiConv, m_eventConv, m_animationConv;
	std::condition_variable m_fixedControlConv, m_physicControlConv, m_blockControlUpdate, m_guiControlConv, m_eventControlConv, m_animationControlConv;

	std::recursive_mutex m_eventMutex;


	std::mutex m_mutex;

	std::vector<std::string> m_words;
	std::vector<sf::Event> m_poll;


	World* m_world;

	std::thread m_mainThread, m_fixedUpdateThread, m_buildChunkThread, m_physicThread, m_uiMainThread, m_eventProcessThread, m_animatonThread;
};