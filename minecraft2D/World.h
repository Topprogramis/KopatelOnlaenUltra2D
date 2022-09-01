#pragma once
#include"Chunk.hpp"

struct BlockReplace {
	int chunkInd;
	sf::Vector2i blockPos;
	int blockInd;
};

class World {
public:
	World(sf::RenderWindow* wiondow = nullptr);


	void LoadBlocks();

	void Generate();
	void Move(sf::Vector2f offset);

	void Save();
	void Load();

	void biuldBlock(sf::Vector2f pos, BlockData* bl);


	void Draw();

	void FixedUpdate();

	static BlockData blocks[256];
	static bool WasLoadBlocks;

private:
	Transform m_transform;
	std::vector<Chunk>m_chunks = {};

	sf::RenderWindow* m_window;

	void GenerateLastChunk(int ind);

	std::vector<BlockReplace> m_data = {};

	sf::Texture m_atlas;
};