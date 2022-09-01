#pragma once
#include"Block.h"
#include"Chunk.hpp"
#include"Transform.h"

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
				m_blocks[y][x] = Block(data,Transform(sf::Vector2f(x*Settings::blockSize.x, y * Settings::blockSize.y), sf::Vector2f(Settings::blockSize.x, Settings::blockSize.y)));
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


	void SetBlock(int x, int y, BlockData* block) {
		m_blocks[y][x].SetData(block);

		if (block->isPhisical)
			m_physicalBlocks.push_back(&m_blocks[y][x]);
		Change();
	}

	Transform& GetTransform() {
		return m_transform;
	}

	void SetWorld(Transform* world) {
		m_world = world;
	}

	void Draw(sf::RenderWindow* window) {
		m_shape.setPosition(m_transform.GetPosition() + m_world->GetPosition());
		window->draw(m_shape);
	}

	void Change() {
		m_OnChnange = true;
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

	void Update() {
		if (m_OnChnange) {
			OnChange();
			m_OnChnange = false;
		}
	}

	void FixedUpdate() {
		for (auto& i : m_physicalBlocks) {

			if (i->IsFly()) {
				
				auto blockUnder = FindLocalBlock(i->getPosition()+sf::Vector2f(0,2));
				if (blockUnder.y!=0&&m_blocks[blockUnder.y+1][blockUnder.x].getId()!=0) {
					i->SetFly(false);
					m_blocks[blockUnder.y][blockUnder.x].SetData(i->GetData());
					i->getTransform().SetPostion(sf::Vector2f(i->getPosition().x * Settings::blockSize.x, i->getPosition().y * Settings::blockSize.y));
					Change();
				}
				else {
					i->getTransform().Move(sf::Vector2f(0, 2));
					Change();
				}

			}

			
			if (m_blocks[i->getChunkPosition().y+1][i->getChunkPosition().x].getId() == 0) {
				i->SetFly(true);
				//SetData(m_blocks[i->getChunkPosition().y - 1][i->getChunkPosition().x].GetData());
				//m_flyingObjects.push_back(i);
				Change();
			}
		}
	}

private:
	Transform m_transform;
	Transform* m_world;

	sf::RectangleShape m_shape;
	sf::Texture* m_atlas;

	sf::Vector2f m_textureOnAtlasSize;

	sf::RenderTexture m_tx;

	bool m_OnChnange = false;

	void OnChange() {
		m_tx.clear(sf::Color::Blue);

		sf::RectangleShape shape (Settings::blockSize);
		shape.setTexture(m_atlas);

		for (int y = 0; y < chunkSizeY; y++) {
			for (int x = 0; x < chunkSizeX; x++) {
				shape.setTextureRect(sf::IntRect(Settings::textureResolution.x * (m_blocks[y][x].getId() % Settings::atlasSize.x), Settings::textureResolution.y * std::floor(m_blocks[y][x].getId() / Settings::atlasSize.y), Settings::textureResolution.x, Settings::textureResolution.y));
				shape.setPosition(m_blocks[y][x].getPosition());

				m_tx.draw(shape);
			}
		}
		for (auto& i : m_flyingObjects) {
			shape.setTextureRect(sf::IntRect(Settings::textureResolution.x * (i->getId() % Settings::atlasSize.x), Settings::textureResolution.y * std::floor(i->getId() / Settings::atlasSize.y), Settings::textureResolution.x, Settings::textureResolution.y));
			shape.setPosition(i->getPosition());
			m_tx.draw(shape);
		}

		m_tx.display();

		m_shape.setTexture(&m_tx.getTexture());
	}

	Block m_blocks[chunkSizeY][chunkSizeX];
	std::vector<Block*> m_physicalBlocks;
	std::vector<Block*> m_flyingObjects;
};