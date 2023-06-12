#pragma once
#include"Block.h"
#include"Chunk.hpp"
#include"Transform.h"
#include"Action.h"
#include"Triangulate.h"

class Chunk {
public:
	Chunk(sf::Texture* atlas = nullptr,Transform transform = Transform()) {

		transform.SetSize(sf::Vector2f(Settings::blockSize.x * chunkSizeX, Settings::blockSize.y * chunkSizeY));
		m_transform = transform;


		//m_tx.create(transform.GetSize().x, transform.GetSize().y);
		//m_shape = sf::RectangleShape(transform.GetSize());

		m_atlas = atlas;

		BlockData* data = new BlockData{ 0 };
		for (int y = 0; y < chunkSizeY; y++) {
			for (int x = 0; x < chunkSizeX; x++) {
				m_blocks[y][x] = Block(data,Transform(sf::Vector2f(x*Settings::blockSize.x, y * Settings::blockSize.y), sf::Vector2f(Settings::blockSize.x, Settings::blockSize.y)),this);
				m_blocks[y][x].setChunkPosition(sf::Vector2i(x, y));
			}
		}
	} 
	Chunk(const Chunk& chunk) {
		this->m_transform = chunk.m_transform;
		this->m_atlas = chunk.m_atlas;

		m_transform.SetSize(sf::Vector2f(Settings::blockSize.x * chunkSizeX, Settings::blockSize.y * chunkSizeY));
	}

	void operator=(const Chunk& chunk) {
		this->m_transform = chunk.m_transform;
		this->m_atlas = chunk.m_atlas;

		m_transform.SetSize(sf::Vector2f(Settings::blockSize.x * chunkSizeX, Settings::blockSize.y *chunkSizeY));
    }

	//chunk id in world
	void SetId(int id) {
		m_id = id;
	}
	int GetId() {
		return m_id;
	}

	//init
	void Init() {
		Change();
	}

	//transform
	Transform& GetTransform() {
		return m_transform;
	}
	sf::Vector2f getPosition() {
		return m_transform.GetPosition();
	}
	void SetWorld(Transform* world) {
		m_world = world;
	}
	Transform* getWorld() {
		return m_world;
	}
	
	//texture
	sf::Texture* getAtlas() {
		return m_atlas;
	}

	//main
	void Draw(sf::RenderWindow* window) {
		sf::RenderStates states;
		states.texture = m_atlas;
		states.transform.translate(m_transform.GetPosition() + m_world->GetPosition());

		window->draw(m_vertexArray, states);
	}
	void Update() {
		BuildChunk();
	}
	void FixedUpdate() {

	}

	//chunk change
	void Change() {
		std::unique_lock<std::mutex> lock(m_mutex);
		
		m_OnChnange = true;

		lock.unlock();
		m_conv.notify_one();	
	}
	bool IsChange() {
		return m_OnChnange;
	}

	//col change
	void ChangeCol() {
		std::unique_lock<std::mutex> lock(m_mutex2);

		m_OnCollisionChange = true;

		lock.unlock();
		m_conv2.notify_one();
	}
	bool IsColChange() {
		return m_OnCollisionChange;
	}

	//finding blocks
	sf::Vector2i FindBlock(sf::Vector2f pos) {
		for (int y = 0; y < chunkSizeY; y++) {
			for (int x = 0; x < chunkSizeX; x++) {
				sf::Vector2f blockPosition = { x * Settings::blockSize.x,y * Settings::blockSize.y };

				blockPosition.y += m_transform.GetPosition().y;
				blockPosition.y += m_world->GetPosition().y;

				blockPosition.x += m_transform.GetPosition().x;
				blockPosition.x += m_world->GetPosition().x;

				

				if (blockPosition.x + Settings::blockSize.x >= pos.x && blockPosition.x <= pos.x &&
					blockPosition.y + Settings::blockSize.y >= pos.y && blockPosition.y <= pos.y) {
					return{ x,y };
				}
			}
		}

		return{ 0,0 };
	}
	sf::Vector2i FindLocalBlock(sf::Vector2f pos) {
		for (int y = 0; y < chunkSizeY; y++) {
			for (int x = 0; x < chunkSizeX; x++) {
				sf::Vector2f blockPosition = { x * Settings::blockSize.x,y * Settings::blockSize.y };



				if (blockPosition.x + Settings::blockSize.x >= pos.x && blockPosition.x <= pos.x &&
					blockPosition.y + Settings::blockSize.y >= pos.y && blockPosition.y <= pos.y) {
					return { x,y };
				}
			}
		}

		return {0,0};
	}

	//block operation
	void SetBlock(int x, int y, BlockData* block) {
		m_blocks[y][x].SetData(block);

		if (m_blocks[y][x].component != nullptr)
			m_interactiableBlocks.push_back(&m_blocks[y][x]);

		Change();
	}
	void SetBlock(sf::Vector2i pos, BlockData* block) {
		m_blocks[pos.y][pos.x].SetData(block);

		if (m_blocks[pos.y][pos.x].component != nullptr)
			m_interactiableBlocks.push_back(&m_blocks[pos.y][pos.x]);
		Change();
	}
	void SetBlock(int x, int y, Block block) {
		m_blocks[y][x] = block;

		if (m_blocks[y][x].component != nullptr)
			m_interactiableBlocks.push_back(&m_blocks[y][x]);
		Change();
	}
	Block* GetBlock(int x, int y) {
		return &m_blocks[y][x];
	}

	//chunk rebilding
	void BuildChunk() {
		if (m_OnChnange) {
			std::unique_lock<std::mutex> lock(m_mutex);
			//m_conv.wait(lock);
			OnChange();
			lock.unlock();
			

			m_OnChnange = false;

			//->Invoke(this);
		}

	}

	//physics
	void PhysicUpdate() {
		if (m_OnCollisionChange) {
			std::unique_lock<std::mutex> lock(m_mutex2);

			OnCollisionChange();

			lock.unlock();
			m_OnCollisionChange = false;
		}
	}
	//update interact blcoks
	void UpdateBlocks() {
		for (int i = 0; i < m_interactiableBlocks.size(); i++) {
			if (m_interactiableBlocks[i]->component)
				m_interactiableBlocks[i]->component->Update(m_interactiableBlocks[i], this, i);
			else
				m_interactiableBlocks.erase(m_interactiableBlocks.begin() + i);

		}
	}

	std::vector<Block*>* getInteractibleBlockListPtr() {
		return &m_interactiableBlocks;
	}

private:
	Transform m_transform;
	Transform* m_world;

	int m_id;

	//drawing
	sf::Texture* m_atlas;
	sf::VertexArray m_vertexArray;

	//generate body
	std::vector<b2Body*> m_bodyList;
	std::vector<b2BodyDef*> m_bodyDefs = {};
	std::vector<b2EdgeShape> m_physicShapes;
	b2BodyDef* CreatBodyDef(Block* block) {
		auto bodyDef = new b2BodyDef();
		bodyDef->type = b2_staticBody;
		bodyDef->position = toGlPosition(m_transform.GetPosition() + sf::Vector2f(block->getChunkPosition().x * Settings::blockSize.x, block->getChunkPosition().y * Settings::blockSize.y));
		return bodyDef;
	}

	//blocks
	Block m_blocks[chunkSizeY][chunkSizeX];
	std::vector<Block* > m_interactiableBlocks;
	std::vector<Block*> m_flyingObjects;

	

	//multy threading
	std::mutex m_mutex, m_mutex2;
	std::condition_variable m_conv, m_conv2;

	//rebuild
	bool m_OnChnange = false, m_OnCollisionChange = false;

	void OnChange();
	void OnCollisionChange();

	//has near blocks
	bool hasNBlockDown(int x, int y) {
		return (y < chunkSizeY -1 && m_blocks[y + 1][x].getId() == 0);
	}
	bool hasNBlockUp(int x, int y) {
		return (y > 0 && m_blocks[y - 1][x].getId() == 0);
	}
	bool hasNBlockLeft(int x, int y);
	bool hasNBlockRight(int x, int y);

	//colisions
	struct BlockCollision {
		Block* block;
		
		bool left, right, up, down;
	};

	std::vector<BlockCollision> m_currentCollisions;
};