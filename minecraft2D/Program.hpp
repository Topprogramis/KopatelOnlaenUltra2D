#pragma once
#include "World.h"
#include"TimeManage.h"
#include"ui/GuiManager.hpp"
#include"BlockController.h"
#include"ui/Animator.hpp"
#include"ThreadManagaer.hpp"
#include "LogWriteCommand.hpp"

class Program {
public:
	Program() {
		Init();
	}

	void Init() {
		Logger();


		LoadSettings();

		m_threadManager = new ThreadManager();

		Settings::threadManager = m_threadManager;

		m_threadManager->AddThread(new Thread("log", std::bind(&Program::LogThread, this, std::placeholders::_1)));
		m_threadManager->AddThread(new Thread("main", std::bind(&Program::Loop, this, std::placeholders::_1)));
		m_threadManager->AddThread(new Thread("uiRender", std::bind(&Program::GuiLoop, this, std::placeholders::_1)));
		m_threadManager->AddThread(new Thread("fixedUpdate", std::bind(&Program::FixedUpdate, this, std::placeholders::_1)));
		m_threadManager->AddThread(new Thread("chunkUpdate", std::bind(&Program::ChunkBlocksUpdate, this, std::placeholders::_1)));
		m_threadManager->AddThread(new Thread("physicUpdate", std::bind(&Program::PhysicUpdate, this, std::placeholders::_1)));
		m_threadManager->AddThread(new Thread("animation", std::bind(&Program::AnimationUpdate, this, std::placeholders::_1)));
	

		Start();

		Time::globalTimeManager = &m_time;

		m_initBarier.CompliteWork();

		m_mainLock.UnLock();

		m_threadManager->LaunchAll();

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
		m_inventory->Load();
	}


	void InitThreads() {
		m_threadManager->AddCommand("log", new LogWriteCommand(Log("------INIT THREADS------", Log::LogType::successful)));

		m_animLock.UnLock();
		m_guiLock.UnLock();
		m_chunkUphateLock.UnLock();
		m_physicLock.UnLock();
		m_fixedUpdateLock.UnLock();
		m_loggerLock.UnLock();

		m_initBarier.Wait(7);
	}

	void Loop(Thread* thread) {
		m_mainLock.Wait();

		InitThreads();

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("Creat window", Log::LogType::successful)));

		CreateWindow();

		m_launchBarier.Wait(6);

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("------LAUNCH THREADS------", Log::LogType::successful)));

		std::cout << "Launch thread init successful" << std::endl;

		m_guiLock.UnLock();
		m_animLock.UnLock();
		m_chunkUphateLock.UnLock();
		m_physicLock.UnLock();
		m_fixedUpdateLock.UnLock();
		m_loggerLock.UnLock();


		std::cout << "Main thread init successful" << std::endl;

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("Main thread init successful", Log::LogType::successful)));

		StartAfterInit();


		while (m_run || m_fixedRun || m_chunkRun || m_physicRun) {
			thread->ResetClock();

			m_animLock.UnLock();
			m_chunkUphateLock.UnLock();
			m_physicLock.UnLock();
			m_fixedUpdateLock.UnLock();
			m_guiLock.UnLock();
			m_loggerLock.UnLock();

			m_sinhBarier.Wait(6,200);

			thread->ExcuteCommands();

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

			m_endWorkThreadCount = 0;

			std::this_thread::sleep_for(std::chrono::milliseconds(std::max(1,Settings::fixedUpdateTime - (int)thread->getElapseTime())));

			m_deltaTime = (float)thread->getElapseTime() / 100.f;

		}

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("Main thread end", Log::LogType::successful)));;

		m_world->Save();

		m_window.close();

		//d::terminate();
	}

	void GuiLoop(Thread* thread) {
		std::cout << "gui start" << std::endl;

 
		m_initBarier.CompliteWork();
		m_guiLock.Wait();

		std::cout << "GUI thread init successful" << std::endl;
		
		InitGui();

		std::cout << "GUI thread init successful" << std::endl;

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("GUI thread init successful", Log::LogType::successful)));

		m_launchBarier.CompliteWork();
		m_guiLock.Wait();

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("GUI thread launch successful", Log::LogType::successful)));

		while (m_run)
		{
			thread->ResetClock();

			thread->ExcuteCommands();

			m_GUImanager->Update();
			m_GUImanager->DrawUiElements();

			m_sinhBarier.CompliteWork();
			m_guiLock.Wait();
		}

	    m_threadManager->AddCommand("log", new LogWriteCommand(Log("GUI thread end", Log::LogType::successful)));
	}
	void AnimationUpdate(Thread* thread) {
		m_initBarier.CompliteWork();
		m_animLock.Wait();


		m_animators = {};


		m_threadManager->AddCommand("log", new LogWriteCommand(Log("Animation update thread init successful", Log::LogType::successful)));

		m_launchBarier.CompliteWork();
		m_animLock.Wait();

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("Animation update thread launch successful", Log::LogType::successful)));

		while (m_fixedRun) {
			thread->ResetClock();

			thread->ExcuteCommands();

			for (auto& i : m_animators)
				i->Process();

			m_sinhBarier.CompliteWork();
			m_animLock.Wait();
		}

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("Animation update thread end", Log::LogType::successful)));
	}
	void LogThread(Thread* thread) {
		m_initBarier.CompliteWork();
		m_loggerLock.Wait();

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("Log update thread init successful", Log::LogType::successful)));

		m_launchBarier.CompliteWork();
		m_loggerLock.Wait();

		Logger::logger->ReOpen();

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("Log update thread launch successful", Log::LogType::successful)));

		while (m_run) {
			thread->ResetClock();

			thread->ExcuteCommands();
	
			m_sinhBarier.CompliteWork();
			m_loggerLock.Wait();
		}

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("Log thread end", Log::LogType::successful)));
	}
	void FixedUpdate(Thread* thread) {
		m_initBarier.CompliteWork();
		m_fixedUpdateLock.Wait();

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("Fixed update thread init successful", Log::LogType::successful)));

		m_launchBarier.CompliteWork();
		m_fixedUpdateLock.Wait();

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("Fixed update thread launch successful", Log::LogType::successful)));

		while (m_fixedRun) {
			thread->ResetClock();

			thread->ExcuteCommands();

			m_world->FixedUpdate();
			m_blockController->FixedUpdate();
			m_GUImanager->FixedUpdate();

			if (m_interact) {
				//m_world->Interact();
				m_interact = false;
			}

			m_sinhBarier.CompliteWork();
			m_fixedUpdateLock.Wait();

		}

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("Fixed update thread end", Log::LogType::successful)));
	}
	void ChunkBlocksUpdate(Thread* thread) {
		m_initBarier.CompliteWork();
		m_chunkUphateLock.Wait();

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("Chunk update thread init successful", Log::LogType::successful)));

		m_launchBarier.CompliteWork();
		m_chunkUphateLock.Wait();

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("Chunk update thread lauch successful", Log::LogType::successful)));

		while (m_chunkRun) {
			thread->ResetClock();

			thread->ExcuteCommands();
			m_world->UpdateBlocks();

			//std::this_thread::sleep_for(std::chrono::milliseconds(Settings::fixedUpdateTime - (int)thread->getElapseTime()));

			m_sinhBarier.CompliteWork();
			m_chunkUphateLock.Wait();
		}

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("chunk update end", Log::LogType::successful)));
	}
	void PhysicUpdate(Thread* thread) {
		m_initBarier.CompliteWork();
		m_physicLock.Wait();

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("physic thread init successful", Log::LogType::successful)));

		m_launchBarier.CompliteWork();
		m_physicLock.Wait();

		m_threadManager->AddCommand("log", new LogWriteCommand(Log("physic thread lauch successful", Log::LogType::successful)));

		while (m_physicRun) {
			thread->ResetClock();

			thread->ExcuteCommands();

			m_world->PhysicUpdate();

			m_sinhBarier.CompliteWork();
			m_physicLock.Wait();
		}

		//m_threadManager->AddCommand("log", new LogWriteCommand(Log("physic update end", Log::LogType::successful)));
	}

	void Update() {

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

	void InitGui() {
		m_GUImanager = new GuiManager(sf::Vector2f(0, 0), Settings::windowSize, { 0,0,0,0 });
		m_GUImanager->setWindow(&m_window);

		m_GUImanager->AddElemnt(new Inventory(Transform(sf::Vector2f(200, 100), sf::Vector2f(600, 600)), 50, 15,{ 9,4 }, "PlayerInventory"));
		m_GUImanager->AddElemnt(new HotBar(Transform(sf::Vector2f(10, 10), sf::Vector2f(500, 60)), 50, 5, { 9,1 }, "HotBar"));

		m_hotBar = static_cast<HotBar*>(m_GUImanager->getElementByName("HotBar"));
		m_inventory = static_cast<Inventory*>(m_GUImanager->getElementByName("PlayerInventory"));
		m_world->getPlayer()->SetInventory(m_inventory);

		m_inventory->BindHotBar(m_hotBar);

		m_blockController = new BlockController(m_inventory);
	}
	void CreateWindow() {
		sf::ContextSettings settings;
		settings.antialiasingLevel = 0.0;
		m_window.create(sf::VideoMode(Settings::windowSize.x, Settings::windowSize.y), "minecraft2D",7, settings);
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

	ThreadManager* m_threadManager;

	Inventory* m_inventory;
	HotBar* m_hotBar;
	BlockController* m_blockController;
	GuiManager* m_GUImanager;

	std::atomic<int> m_endWorkThreadCount = 0;

	std::vector<Animator*> m_animators;

	std::map<std::string, std::string> m_settings;

	sf::Texture m_atlas;

	bool m_interact = false;

	bool m_run = true;

	bool m_fixedRun = true, m_chunkRun = true, m_physicRun = true;

	float m_deltaTime =0.0f;

	
	ThreadBarier m_initBarier, m_launchBarier, m_sinhBarier;
	ThreadLock m_animLock, m_guiLock, m_physicLock, m_fixedUpdateLock, m_chunkUphateLock, m_mainLock, m_loggerLock;

	


	std::recursive_mutex m_eventMutex;


	std::mutex m_mutex;

	std::vector<std::string> m_words;
	std::vector<sf::Event> m_poll;


	World* m_world;
};