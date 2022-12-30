#pragma once
#include"Block.h"
#include"Chunk.hpp"
#include"Transform.h"
#include"Action.h"

class Chunk {
public:
	Chunk(sf::Texture* atlas = nullptr,Transform transform = Transform()) {

		transform.SetSize(sf::Vector2f(Settings::blockSize.x * chunkSizeX, Settings::blockSize.y * chunkSizeY));
		m_transform = transform;


		m_tx.create(transform.GetSize().x, transform.GetSize().y);
		m_shape = sf::RectangleShape(transform.GetSize());

		m_atlas = atlas;

		BlockData* data = new BlockData{ 0 };
		for (int y = 0; y < chunkSizeY; y++) {
			for (int x = 0; x < chunkSizeX; x++) {
				m_blocks[y][x] = Block(data,Transform(sf::Vector2f(x*Settings::blockSize.x, y * Settings::blockSize.y), sf::Vector2f(Settings::blockSize.x, Settings::blockSize.y)),this);
				m_blocks[y][x].setChunkPosition(sf::Vector2i(x, y));
			}
		}


		OnChange();
	} 
	Chunk(const Chunk& chunk) {
		this->m_transform = chunk.m_transform;
		this->m_atlas = chunk.m_atlas;

		m_transform.SetSize(sf::Vector2f(Settings::blockSize.x * chunkSizeX, Settings::blockSize.y * chunkSizeY));

		m_tx.create(m_transform.GetSize().x, m_transform.GetSize().y);
		m_shape.setSize(m_transform.GetSize());



		Change();
	}

	void operator=(const Chunk& chunk) {
		this->m_transform = chunk.m_transform;
		this->m_atlas = chunk.m_atlas;

		m_transform.SetSize(sf::Vector2f(Settings::blockSize.x * chunkSizeX, Settings::blockSize.y *chunkSizeY));

		m_tx.create(m_transform.GetSize().x, m_transform.GetSize().y);
		m_shape.setSize(m_transform.GetSize());




		Change();
   }

	void SetId(int id) {
		m_id = id;
	}
	int GetId() {
		return m_id;
	}

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
	
	sf::Texture* getAtlas() {
		return m_atlas;
	}

	void Draw(sf::RenderWindow* window) {
		m_shape.setPosition(m_transform.GetPosition() + m_world->GetPosition());
		window->draw(m_shape);
	}
	void Update() {
		BuildChunk();
	}
	void FixedUpdate() {

	}

	void Change() {
		std::unique_lock<std::mutex> lock(m_mutex);
		
		m_OnChnange = true;

		lock.unlock();
		m_conv.notify_one();	
	}
	bool IsChange() {
		return m_OnChnange;
	}


	void Interact(sf::Vector2i pos) {
		auto blockPos = FindBlock(sf::Vector2f(pos));
		auto block = &m_blocks[blockPos.y][blockPos.x];
			if(block->GetData()->component!=nullptr)
				block->component->OnInteract(block, this);

	}

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

	sf::Texture* m_atlas;
	sf::RenderTexture m_tx;
	sf::Vector2f m_textureOnAtlasSize;

	sf::RectangleShape m_shape;

	Block m_blocks[chunkSizeY][chunkSizeX];
	std::vector<Block*> m_interactiableBlocks;
	std::vector<Block*> m_flyingObjects;

	bool m_OnChnange = false;

	std::mutex m_mutex;
	std::condition_variable m_conv;

	void OnChange() {

		m_tx.clear(sf::Color(0,0,0,0));

		sf::RectangleShape shape (Settings::blockSize);
		shape.setTexture(m_atlas);

		for (int y = 0; y < chunkSizeY; y++) {
			for (int x = 0; x < chunkSizeX; x++) {
				shape.setTextureRect(sf::IntRect(Settings::textureResolution.x * (m_blocks[y][x].getId() % Settings::atlasSize.x), Settings::textureResolution.y * std::floor(m_blocks[y][x].getId() / Settings::atlasSize.y), Settings::textureResolution.x, Settings::textureResolution.y));
				shape.setPosition(m_blocks[y][x].getPosition());

				m_tx.draw(shape);
			}
		}		

		m_tx.display();

		m_shape.setTexture(&m_tx.getTexture());
	}


};