#include"World.h"

BlockData World::blocks[256] = {};
bool World::WasLoadBlocks = false;

World::World(sf::RenderWindow* window) {
	m_window = window;
	if (!WasLoadBlocks) {
		LoadBlocks();
	}

	m_chunks.resize(Settings::worldSize);

	m_atlas.loadFromFile("atlas.png");

	m_transform.SetPostion(sf::Vector2f(-(chunkSizeX * Settings::blockSize.x * Settings::worldSize) / 2, 0));

	Generate();

	Load();
}

void World::LoadBlocks() {
	for (int i = 0; i < 256; i++) {
		blocks[i] = BlockData{ i };
	}

	blocks[18].isPhisical = true;
	blocks[19].isPhisical = true;

	WasLoadBlocks = true;
}

void World::Generate() {
	for (int i = 0; i < Settings::worldSize; i++) {
		m_chunks[i]=Chunk(&m_atlas, Transform(sf::Vector2f(Settings::blockSize.x * chunkSizeX * i, -(Settings::blockSize.y * chunkSizeY / 2))));
		m_chunks[i].SetWorld(&m_transform);
		GenerateLastChunk(i);
	}
}

void World::Move(sf::Vector2f offset) {
	m_transform.SetPostion(m_transform.GetPosition() + offset);
}

void World::Draw() {
	for (int i = 0; i < Settings::worldSize; i++) {
		m_chunks[i].Update();
		m_chunks[i].Draw(m_window);
	}
}

void World::FixedUpdate() {
	for (int i = 0; i < Settings::worldSize; i++) {
		m_chunks[i].FixedUpdate();
	}
}

void World::biuldBlock(sf::Vector2f pos, BlockData* bl){
	for (int i = 0; i < Settings::worldSize; i++) {
		if (m_chunks[i].GetTransform().GetPosition().x<pos.x - m_transform.GetPosition().x && m_chunks[i].GetTransform().GetPosition().x + m_chunks[i].GetTransform().GetSize().x > pos.x - m_transform.GetPosition().x &&
			m_chunks[i].GetTransform().GetPosition().y<pos.y - m_transform.GetPosition().y && m_chunks[i].GetTransform().GetPosition().y + m_chunks[i].GetTransform().GetSize().y > pos.y - m_transform.GetPosition().y) {

			auto p = m_chunks[i].FindBlock(pos);

			BlockReplace upd;
			upd.blockInd = bl->id;
			upd.blockPos = p;
			upd.chunkInd = i;


			m_data.push_back(upd);
			m_chunks[i].SetBlock(p.x, p.y, bl);

			break;
		}
	}
}

void World::GenerateLastChunk(int ind) {
	Chunk* chunk = &m_chunks[ind];

	for (int y = 0; y < chunkSizeY; y++) {
		for (int x = 0; x < chunkSizeX; x++) {
			int blockId =0;
			
			if (y > 72) {
				blockId = 1;
			}
			else if (y == 68) {
				blockId = 3;
			}
			else if (y > 68) {
				blockId = 2;
			}
			


			chunk->SetBlock(x, y, &blocks[blockId]);
		}
	}


}

void World::Save() {
	std::ofstream saveFile;

	saveFile.open("saves\\world.txt");
	if (!saveFile.is_open()) {
		std::cout << "Save is no open. Please create in game files saves/world.txt" << std::endl;
		return;
	}

	//saveFile.clear();

	for (auto& i : m_data) {
		std::string data = "";

		data += "chunk(" + std::to_string(i.chunkInd) + ");";
		data += "pos(" + std::to_string(i.blockPos.x) + "," + std::to_string(i.blockPos.y) + ");";
		data += "block(" + std::to_string(i.blockInd) + ")\n";

		saveFile << data;
	}

	saveFile.close();


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

		for (int i = 0; i < m_data.size(); i++) {
			if (m_data[i].blockPos == pos && m_data[i].chunkInd == chunkId) {
				m_data[i] = { chunkId,pos,blockId };
			}
		}
		m_chunks[chunkId].SetBlock(pos.x, pos.y, &blocks[blockId]);
	}
}