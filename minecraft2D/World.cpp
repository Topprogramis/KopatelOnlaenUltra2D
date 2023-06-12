#include"World.h"
#include"PhysicalBlock.h"
#include"MusicBlock.h"
#include"BombBlock.h"
#include"FlyingBlock.h"
#include"Perlin.hpp"
#include"ContactListener.hpp"

BlockData World::blocks[256] = {};
bool World::WasLoadBlocks = false;

World::World(sf::RenderWindow* window) {
	m_window = window;

	if (!WasLoadBlocks) {
		std::cout << "Loading blocks..." << std::endl;
		LoadBlocks();
	}


	m_chunks.resize(Settings::worldSize);

	m_atlas.loadFromFile("atlas.png");
	m_skyBoxTx.loadFromFile("sky.png");

	m_skyBoxShape.setSize(Settings::windowSize);
	m_skyBoxShape.setTexture(&m_skyBoxTx);
	
	m_OnChunkBuild.AddListener(std::bind(&World::OnChunkBuild, this, std::placeholders::_1));

	m_physicalWorld = new b2World(b2Vec2(0, Settings::gravy));
	m_physicalWorld->SetContactListener(new ContactListener());
	
	Settings::world = this;

	m_player = new Player(sf::Vector2f(-(chunkSizeX * Settings::blockSize.x * Settings::worldSize) / 2, 0));

	std::cout << "Terrarion generation..." << std::endl;
	std::cout << "--------------chunks--------------" << std::endl;
	
	Generate();

	std::cout << "Load world data..." << std::endl;

	Load();

	std::cout << "Spawn player" << std::endl;

	m_player->InitInWorld();
}

Chunk* World::getChunk(int chunkId) {
	return &m_chunks[chunkId];
}


void World::LoadBlocks() {
	for (int i = 0; i < 256; i++) {
		blocks[i] = BlockData{ i };
	}

	blocks[8].component = new BombBlock(8,3,false);

	blocks[18].component = new PhysicalComponent();
	blocks[19].component = new PhysicalComponent();

	blocks[74].component = new MusicBlock();

	WasLoadBlocks = true;
}
void World::Generate() {
	for (int i = 0; i < Settings::worldSize; i++) {
		m_chunks[i]=Chunk(&m_atlas, Transform(sf::Vector2f(Settings::blockSize.x * chunkSizeX * i, -(Settings::blockSize.y * chunkSizeY / 2))));
		
		m_chunks[i].SetId(i);
		m_chunks[i].SetWorld(m_player->getTransform());

		GenerateLastChunk(i);

		std::cout << "chunk " + std::to_string(i+1) + " was generated" << std::endl;
	}
	std::cout << "----------------------------------" << std::endl;
}


std::vector<FlyingBlock*>* World::getFlyingObjectsListPtr() {
	return &m_flyingBlocks;
}
void World::Interact(Block* bl) {
	if (bl->component != nullptr)
		bl->component->OnInteract(bl,bl->getChunk());
}
void World::Move(sf::Vector2f offset) {
	//.m_transform.SetPostion(m_transform.GetPosition() + offset);
}


void World::Draw() {
	m_window->draw(m_skyBoxShape);
	//sf::RectangleShape sp = sf::RectangleShape({ 80, 80 });
	//sp.setPosition({ body->GetPosition().x + m_player->getPosition().x ,body->GetPosition().y + m_player->getPosition().y});


	for (int i = 0; i < Settings::worldSize; i++) {
		m_chunks[i].Update();
		m_chunks[i].Draw(m_window);
	}




	std::unique_lock<std::mutex> lock(w_mutex);

	for (auto& i : m_flyingBlocks) {
		i->getShapePtr()->setPosition(i->getBlock()->getPosition()+i->getBlock()->getChunk()->getPosition()+ m_player->getPosition());
		m_window->draw(*i->getShapePtr());
	}

	m_player->Draw(m_window);

	lock.unlock();
	condition.notify_one();


}
void World::FixedUpdate() {
	m_player->FixedUpdate();

	for (int i = 0; i < Settings::worldSize; i++) {
		m_chunks[i].FixedUpdate();
	}
	m_cv.notify_one();
}


//block
void World::UpdateBlocks() {

	for (auto& i : m_chunks) {
		i.UpdateBlocks();
	}
}

void World::DestroyBlock(Block* currentBl) {
	BlockReplace upd;
	upd.blockInd = 0;
	upd.blockPos = currentBl->getChunkPosition();
	upd.chunkInd = currentBl->getChunk()->GetId();

	OnBlockBreak.Invoke(currentBl->GetData());


	m_data.push_back(upd);
	m_chunks[upd.chunkInd].SetBlock(upd.blockPos.x, upd.blockPos.y, &blocks[0]);
}


Block* World::findBlock(sf::Vector2f pos) {
	for (int i = 0; i < Settings::worldSize; i++) {
		if (m_chunks[i].GetTransform().GetPosition().x<pos.x - m_player->getPosition().x && m_chunks[i].GetTransform().GetPosition().x + m_chunks[i].GetTransform().GetSize().x > pos.x - m_player->getPosition().x &&
			m_chunks[i].GetTransform().GetPosition().y<pos.y - m_player->getPosition().y && m_chunks[i].GetTransform().GetPosition().y + m_chunks[i].GetTransform().GetSize().y > pos.y - m_player->getPosition().y) {

			auto p = m_chunks[i].FindBlock(pos);
			auto block = m_chunks[i].GetBlock(p.x, p.y);

			return block;
		}
	}
}

bool World::buildBlock(Block* currentBl, BlockData* bl){
	auto list = m_flyingBlocks;
	for (int i = 0; i < m_flyingBlocks.size(); i++) {
		if (m_flyingBlocks.at(i)->getBlock()->getPosition().x + Settings::blockSize.x / 2 >= currentBl->getPosition().x &&
			m_flyingBlocks.at(i)->getBlock()->getPosition().x <= currentBl->getPosition().x + Settings::blockSize.x / 2 &&
			m_flyingBlocks.at(i)->getBlock()->getPosition().y + Settings::blockSize.y / 2 >= currentBl->getPosition().y &&
			m_flyingBlocks.at(i)->getBlock()->getPosition().y <= currentBl->getPosition().y + Settings::blockSize.y / 2) {
			return false;
		}
	}

	if (currentBl->GetData()->id != 0) {
		return false;
	}

	BlockReplace upd;
	upd.blockInd = bl->id;
	upd.blockPos = currentBl->getChunkPosition();
	upd.chunkInd = currentBl->getChunk()->GetId();


	m_data.push_back(upd);
	m_chunks[upd.chunkInd].SetBlock(upd.blockPos.x, upd.blockPos.y, bl);
}
void World::SetBlock(sf::Vector2i pos, BlockData* bl) {
	int chunk = std::floor(pos.x / 16);
	pos.x -= chunk * 16;

	m_chunks[chunk].SetBlock(pos, bl);
}
Block* World::GetBlock(sf::Vector2i pos) {
	int chunk = std::floor(pos.x / 16);
	pos.x -= chunk * 16;

	return m_chunks[chunk].GetBlock(pos.x,pos.y);
}

b2Body* World::creatBody(b2BodyDef* def) {
	return m_physicalWorld->CreateBody(def);
}
b2Joint* World::creatJoint(b2JointDef* def) {
	return m_physicalWorld->CreateJoint(def);
}

//chunks
bool World::IsAllChunksBuild() {
	for (auto& i : m_chunks) {
		if (i.IsChange())
			return false;
	}
	return true;
}
void World::OnChunkBuild(Chunk* chunk) {
	m_cv.notify_one();
}
void World::GenerateLastChunk(int ind) {
	Chunk* chunk = &m_chunks[ind];

	auto noise = PerlinNoise(Settings::persistence, Settings::frequency, Settings::amplitude, Settings::octaves, Settings::seed);

	int oldHeight = 70;

	for (int x = 0; x < chunkSizeX; x++) {
		int grassHeight;
		for (int y = 0; y < chunkSizeY; y++) {
			int blockId = 0;

			grassHeight = int(noise.GetHeight(x+ind*16, 0)) + oldHeight;

			if (y > grassHeight + 4) {
				blockId = 1;
			}
			else if (y > grassHeight) {
				blockId = 2;
			}
			else if (y == grassHeight) {
				blockId = 3;
			}
			


			chunk->SetBlock(x, y, &blocks[blockId]);
		}
		oldHeight = grassHeight;
	}
}
void World::BuildChunks() {
	for (auto& i : m_chunks) {
		i.BuildChunk();
	}
	//m_cv.notify_one();
}
void World::PhysicUpdate() {
	m_physicalWorld->Step(Settings::fixedUpdateTime / 100.f, 1, 1);
	for (auto& i : m_chunks) {
		i.PhysicUpdate();
	}
	//m_cv.notify_one();
}

Player* World::getPlayer() {
	return m_player;
}
//world saves
void World::Save() {
	std::ofstream saveFile;

	saveFile.open("saves\\world.txt");
	if (!saveFile.is_open()) {
		std::cout << "Save is no open. Please create in game files saves/world.txt" << std::endl;
		return;
	}

	//saveFile.clear();

	std::vector<BlockReplace> saved;



	std::reverse(m_data.begin(),m_data.end());

	for (auto& i : m_data) {

		bool skip = false;
	
		for (auto& j : saved) {
			if (j.blockPos == i.blockPos && j.chunkInd == i.chunkInd) {
				skip = true;
			}
		}

		if (skip) {
			continue;
		}


			


		std::string data = "";

		data += "chunk(" + std::to_string(i.chunkInd) + ");";
		data += "pos(" + std::to_string(i.blockPos.x) + "," + std::to_string(i.blockPos.y) + ");";
		data += "block(" + std::to_string(i.blockInd) + ")\n";

		saveFile << data;

		saved.push_back(i);	
	}

	saveFile.close();

	std::cout<<"world was saved"<<std::endl;


}
void World::Load() {
	for (auto i : m_chunks) {
		i.Init();
	}

	std::ifstream saveFile;

	saveFile.open("saves\\world.txt");
	if (!saveFile.is_open()) {
		std::cout << "Save is not found. Please create in game files saves/world.txt" << std::endl;
		return;
	}

	std::string buf;
	while (std::getline(saveFile, buf))
	{
		int blockId = 0, chunkId = 0;
		sf::Vector2i pos;

		bool initName = false;

		std::string name = "";
		std::string data = "";
		for (auto& i : buf) {
			if (i == '(') {
				initName = true;
				continue;
			}
			else if (i == ')') {
				if (name == "chunk") {
					chunkId = std::stoi(data);
				}
				else if (name == "pos") {
					std::string x, y;
					bool initX = false;
					for (auto& j : data) {
						if (j == ',') {
							initX = true;
						}
						else {
							if (initX) {
								y += j;
							}
							else {
								x += j;
							}
						}
					}

					pos.x = std::stoi(x);
					pos.y = std::stoi(y);
				}
				else if (name == "block") {
					blockId = std::stoi(data);
				}
			}
			else if (i == ';') {
				initName = false;

				name = "";
				data = "";
			}
			else {
				if (initName) {
					data += i;
				}
				else {
					name += i;
				}
			}
		}

		m_data.push_back(BlockReplace{ chunkId,pos,blockId });
		m_chunks[chunkId].SetBlock(pos.x, pos.y, &blocks[blockId]);
	}
}