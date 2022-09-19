#pragma once
#include"Block.h"
#include"Chunk.hpp"
#include"FlyingBlock.h"
#include"World.h"

class BombBlock : public InteractibleComponent
{
public:
	BombBlock(int bombPower, float detonateTime,bool isDitonate) {
		m_bombPower = bombPower;
		m_detonateTime = detonateTime;
		m_isDitonate = false;
	}

	InteractibleComponent* copy() override {
		return new BombBlock(m_bombPower, m_detonateTime,m_isDitonate);
	}

	int GetPower() {
		return m_bombPower;
	}

	void Update(Block* block, Chunk* chunk,int ind) override {
		if (block->IsFly()) {
			m_time += (float)Settings::fixedUpdateTime / 1000;

			if (int(m_time * 4) % 2) {
				m_flyingBlock.getBlockData()=&Settings::world->blocks[8];
				m_flyingBlock.Update();
			}
			else {
				m_flyingBlock.getBlockData() = &Settings::world->blocks[9];
				m_flyingBlock.Update();
			}

			if (m_time >= m_detonateTime) {				
				block->SetFly(false);

				int selectInd = 0;
				auto list = Settings::world->getFlyingObjectsListPtr();
				for (int i = 0; i < list->size(); i++) {
					if (list->at(i)->getBlock() == m_flyingBlock.getBlock()) {
						selectInd = i;
					}
				}

				std::unique_lock<std::mutex> lock(Settings::world->w_mutex);
				Settings::world->condition.wait(lock);

				//if(Settings::world->getFlyingObjectsListPtr()->size()> selectBlockInd)

				if (selectInd < list->size())
					list->erase(list->begin() + selectInd);

				lock.unlock();

				chunk->Change();

				Explode(block, chunk);
			}
		}
		else if (m_isDitonate) {
			block->SetFly(true);

			std::unique_lock<std::mutex> lock(Settings::world->w_mutex);
			Settings::world->condition.wait(lock);

			m_flyingBlock = FlyingBlock(block, Settings::world->getFlyingObjectsListPtr()->size());

			Settings::world->getFlyingObjectsListPtr()->push_back(&m_flyingBlock);
			
			lock.unlock();

			m_isDitonate = true;

			m_time = 0.0f;
			chunk->Change();
		}
	}
	

	void OnInteract(Block* block, Chunk* chunk) override {
		m_isDitonate = true;
	}

	void Explode(Block* block, Chunk* chunk) {
		auto startPosition = block->getChunkPosition();

		int bombPower = m_bombPower;


		chunk->SetBlock(startPosition.x, startPosition.y, &World::blocks[0]);

		for (int x = -bombPower / 2; x < bombPower / 2; x++) {
			for (int y = -bombPower / 2; y < bombPower / 2; y++) {
				if (x == 0 && y == 0) continue;
				auto a = sf::Vector2i(x, y);

				if (x * x + y * y <= bombPower / 2 * bombPower / 2) {
					Block* bl = Settings::world->GetBlock(startPosition + a + sf::Vector2i(chunk->GetId() * 16, 0));

					if (startPosition.y + a.y > 127 || startPosition.y + a.y < 0)
						continue;

					if (bl->component != nullptr) {
						auto cmp = dynamic_cast<BombBlock*>(bl->component);
						if (cmp != nullptr) {
							cmp->OnInteract(bl, bl->getChunk());
							continue;
						}
					}

					Settings::world->SetBlock(startPosition + a + sf::Vector2i(chunk->GetId() * 16, 0), &World::blocks[0]);
				}
			}
		}
		
	}

	void OnDestroy(Block* block, Chunk* chunk) override {
		block->SetFly(false);

		int selectInd = -1;
		auto list = Settings::world->getFlyingObjectsListPtr();
		for (int i = 0; i < list->size(); i++) {
			if (list->at(i)->getBlock() == m_flyingBlock.getBlock()) {
				selectInd = i;
			}
		}

		std::unique_lock<std::mutex> lock(Settings::world->w_mutex);
		Settings::world->condition.wait(lock);

		//if(Settings::world->getFlyingObjectsListPtr()->size()> selectBlockInd)

		if (selectInd!=0&&selectInd < list->size())
			list->erase(list->begin() + selectInd);

	
		lock.unlock();

		chunk->Change();
	}
private:
	int m_bombPower = 10;
	bool m_isDitonate;

	FlyingBlock m_flyingBlock;

	float m_detonateTime = 3.0f;
	float m_time = 0.0f;
};