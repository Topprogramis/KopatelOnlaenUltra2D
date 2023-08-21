#include"World.h"
#include"PhysicalBlock.h"
#include"MusicBlock.h"
#include"BombBlock.h"
#include"FlyingBlock.h"
#include"Perlin.hpp"
#include"ContactListener.hpp"
#include"ThreadManagaer.hpp"
#include"LogWriteCommand.hpp"
#include"CaveGenerator.h"

BlockData World::blocks[256] = {};
bool World::WasLoadBlocks = false;

World::World(sf::RenderWindow* window) {
	m_window = window;

	if (!WasLoadBlocks) {
		std::cout << "Loading blocks..." << std::endl;
		LoadBlocks();
	}

	LoadGenerationSettings();


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

	Settings::threadManager->AddCommand("log", new LogWriteCommand(Log("Terrain generation")));
	std::cout << "Terrarion generation..." << std::endl;
	std::cout << "--------------chunks--------------" << std::endl;
	
	Generate();

	std::cout << "Load world data..." << std::endl;

	Load();

	std::cout << "Spawn player" << std::endl;

	m_player->InitInWorld();
}

void World::LoadGenerationSettings() {
	std::ifstream file;

	file.open("C:\\Users\\makst\\source\\repos\\Topprogramis\\KopatelOnlaenUltra2D\\minecraft2D\\saves\\generationSettings.txt.txt");
	if (!file.is_open()) {
		std::cout << "Save is not found. Please create in game files saves/world.txt" << std::endl;
		return;
	}

	std::string data = "", buf;
	while (std::getline(file, buf))
	{
		data += buf;
	}

	rapidjson::Document document;
	document.Parse(data.c_str());
	rapidjson::Value& biomes = document["biomes"];
	assert(biomes.IsArray());

	for (rapidjson::SizeType i = 0; i < biomes.Size(); i++) {
		Biome newBiome;
		newBiome.name = biomes[i]["name"].GetString();

		newBiome.persistence = biomes[i]["persistence"].GetInt();
		newBiome.frequency = biomes[i]["frequency"].GetInt();
		newBiome.amplitude = biomes[i]["amplitude"].GetInt();
		newBiome.octaves = biomes[i]["octaves"].GetInt();

		newBiome.layers = {};
		rapidjson::Value& layers = biomes[i]["layers"];
		assert(layers.IsArray());

		for (rapidjson::SizeType layer = 0; layer < layers.Size(); layer++) {
			Layer newLayer;
			newLayer.blockId = biomes[i]["layers"][layer]["blockId"].GetInt();
			newLayer.layerSize = biomes[i]["layers"][layer]["levelSize"].GetInt();
			newBiome.layers.push_back(newLayer);
		}

		newBiome.ores = {};
		rapidjson::Value& ores = biomes[i]["ores"];
		assert(layers.IsArray());

		for (rapidjson::SizeType ore = 0; ore < ores.Size(); ore++) {
			Ore newOre;
			newOre.id = biomes[i]["ores"][ore]["id"].GetInt();
			newOre.maxHeight = biomes[i]["ores"][ore]["maxHeight"].GetInt();
			newOre.minHeight = biomes[i]["ores"][ore]["minHeight"].GetInt();
			newOre.maxSize = biomes[i]["ores"][ore]["maxSize"].GetInt();
			newOre.countPerChunk = biomes[i]["ores"][ore]["countPerChunk"].GetInt();
			
			newBiome.ores.push_back(newOre);
		}

		m_biomes.push_back(newBiome);
	}
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
	srand(Settings::seed);
	for (int i = 0; i < Settings::worldSize; i++) {
		m_chunks[i] = Chunk(&m_atlas, Transform(sf::Vector2f(Settings::blockSize.x * chunkSizeX * i, -(Settings::blockSize.y * chunkSizeY / 2))));

		m_chunks[i].SetId(i);
		m_chunks[i].SetWorld(m_player->getTransform());

		m_chunks[i].SetBiome(m_biomes[0]);
	}

	int oldI = 0;
	for (int i = 0; i < Settings::worldSize; i+=rand()%4) {
		Biome b = m_biomes[rand() % m_biomes.size()];
		for(int j = oldI; j<=i;j++)
			m_chunks[j].SetBiome(b);

		oldI = i;
	}

	for (int i = 0; i < Settings::worldSize; i++) {
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



	for (int i = 0; i < Settings::worldSize; i++) {
		m_chunks[i].Draw(m_window);
	}



	for (auto& i : m_flyingBlocks) {
		i->getShapePtr()->setPosition(i->getBlock()->getPosition()+i->getBlock()->getChunk()->getPosition()+ m_player->getPosition());
		m_window->draw(*i->getShapePtr());
	}

	m_player->Draw(m_window);



}
void World::FixedUpdate() {


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
	if (currentBl == nullptr || currentBl->GetData() == nullptr)
		return false;

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
	Biome currentBiome = chunk->getBiome();

	auto noise = PerlinNoise(currentBiome.persistence, currentBiome.frequency, currentBiome.amplitude, currentBiome.octaves, Settings::seed);

	int oldHeight = 70;

	

	auto caveNoise = PerlinNoise(currentBiome.persistence, currentBiome.frequency, currentBiome.amplitude, 16, Settings::seed);


	for (int x = 0; x < chunkSizeX; x++) {
		int grassHeight = int(noise.GetHeight(x + ind * 16, 0)) + oldHeight;

		for (int y = 0; y < chunkSizeY; y++) {
			if (chunk->GetBlock(x, y)->GetData()->id != 0)
				continue;

			int blockId = 0;

			Layer currentLayer = { 0,0 };

			

			for (auto& layer : currentBiome.layers) {
				if (y >= grassHeight && y <= grassHeight + layer.layerSize) {
					currentLayer = layer;
				}
			}

			blockId = currentLayer.blockId;
			

			chunk->SetBlock(x, y, &blocks[blockId]);
		}
		oldHeight = grassHeight;
	}

	GenerationSettings genS;
	genS.rows = chunkSizeX;
	genS.cols = chunkSizeY;
	genS.generation_count = 30;
	genS.live_chance = 50;
	genS.live_limit = { 4,8 };
	genS.born_limit = { 2,3 };

	//auto cave = GenerateCave(genS);

	//for (int x = 0; x < chunkSizeX; x++) {
	//	for (int y = oldHeight; y < chunkSizeY; y++) {
	//		if (chunk->GetBlock(x, y)->GetData()->id != 0 && cave.data[x+(y* chunkSizeX)]) {
	//			chunk->SetBlock({ x,y }, &blocks[0]);
	//			
	//		}
	//	}
	//}

	for (auto& i : currentBiome.ores) {
		for (int c = 0; c < i.countPerChunk; c++) {
			int blockCount = rand() % (i.maxSize - 1) + 2;
			int x = rand() % chunkSizeX;
			int y = rand() % (i.maxHeight - i.minHeight) + i.minHeight;
			for (int j = 0; j < blockCount; j++) {
				int genType = rand() % 4;
				switch (genType)
				{
				case 0:
					if (y + 1 < chunkSizeY)
						y++;
					else
						y--;
				case 1:
					if (y - 1 >= 0)
						y--;
					else
						y++;
				case 2:
					if (x + 1 < chunkSizeX)
						x++;
					else
						x--;
				case 3:
					if (x - 1 >= 0)
						x--;
					else
						x++;
				}
				chunk->SetBlock(x, y, &blocks[i.id]);
			}
		}


	}

}
void World::BuildChunks() {
	for (auto& i : m_chunks) {
		i.BuildChunk();
	}
	//m_cv.notify_one();
}
void World::PhysicUpdate() {
	m_player->FixedUpdate();

	m_physicalWorld->Step(Settings::fixedUpdateTime / 100.f, 1, 1);

	for (auto& i : m_chunks) {
		i.PhysicUpdate();
	}
}

Player* World::getPlayer() {
	return m_player;
}
//world saves
void World::Save() {
	m_player->SaveData();
	std::ofstream saveFile;

	saveFile.open("C:\\Users\\makst\\source\\repos\\Topprogramis\\KopatelOnlaenUltra2D\\minecraft2D\\saves\\world.txt");
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