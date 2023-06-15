#pragma once
#include"Action.h"
#include"Chunk.hpp"
#include"Player.hpp"

struct BlockReplace {
	int chunkInd;
	sf::Vector2i blockPos;
	int blockInd;
};

class World {
public:
	World(sf::RenderWindow* wiondow = nullptr);

	void LoadBlocks();

	Chunk* getChunk(int chunkId);

	void Generate();

	void BuildChunks();

	Block* findBlock(sf::Vector2f pos);

	bool buildBlock(Block* currentBl, BlockData* bl);
	void DestroyBlock(Block* currentBl);

	void SetBlock(sf::Vector2i pos, BlockData* bl);
	Block* GetBlock(sf::Vector2i pos);

	std::vector<FlyingBlock*>* getFlyingObjectsListPtr();


	void Draw();
	void FixedUpdate();

	void UpdateBlocks();
	void Interact(Block* block);

	void PhysicUpdate();

	bool IsAllChunksBuild();

	void Move(sf::Vector2f offset);

	void Save();
	void Load();

	Player* getPlayer();

	b2Body* creatBody(b2BodyDef* def);
	b2Joint* creatJoint(b2JointDef* def);

	static BlockData blocks[256];
	static bool WasLoadBlocks;

	std::condition_variable condition;
	std::mutex w_mutex;

	sf::VertexArray m_vertexArray;

	Action<BlockData*> OnBlockBreak;

private:
	sf::RenderWindow* m_window;

	std::vector<Biome> m_biomse;

	std::vector<BlockReplace> m_data = {};
	std::vector<Chunk>m_chunks = {};
	std::vector<FlyingBlock*> m_flyingBlocks;

	std::condition_variable m_cv;

	sf::RectangleShape m_skyBoxShape;

	sf::Texture m_atlas, m_skyBoxTx;

	Action<Chunk*> m_OnChunkBuild;

	b2World* m_physicalWorld;

	Player* m_player;

	void GenerateLastChunk(int ind);
	void OnChunkBuild(Chunk* chunk);
};