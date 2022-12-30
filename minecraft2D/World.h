#pragma once
#include"Action.h"
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

	void BuildChunks();

	bool buildBlock(sf::Vector2f pos, BlockData* bl);
	void DestroyBlock(sf::Vector2f pos);
	void SetBlock(sf::Vector2i pos, BlockData* bl);
	Block* GetBlock(sf::Vector2i pos);

	std::vector<FlyingBlock*>* getFlyingObjectsListPtr();


	void Draw();
	void FixedUpdate();

	void UpdateBlocks();
	void Interact();

	bool IsAllChunksBuild();

	void Move(sf::Vector2f offset);

	void Save();
	void Load();

	static BlockData blocks[256];
	static bool WasLoadBlocks;

	std::condition_variable condition;
	std::mutex w_mutex;

private:
	Transform m_transform;
	sf::RenderWindow* m_window;

	std::vector<BlockReplace> m_data = {};
	std::vector<Chunk>m_chunks = {};
	std::vector<FlyingBlock*> m_flyingBlocks;

	std::condition_variable m_cv;

	sf::RectangleShape m_skyBoxShape;

	sf::Texture m_atlas, m_skyBoxTx;

	Action<Chunk*> m_OnChunkBuilt;


	void GenerateLastChunk(int ind);
	void OnChunkBuilt(Chunk* chunk);
};