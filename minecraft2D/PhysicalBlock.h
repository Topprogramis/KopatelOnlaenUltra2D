#pragma once
#include"Block.h"
#include"Chunk.hpp"
#include"World.h"
#include"FlyingBlock.h"

class PhysicalComponent : public InteractibleComponent {
public:
	InteractibleComponent* copy() override {
		return new PhysicalComponent();
	}

	void Update(Block* block, Chunk* chunk, int ind) override {

		if (block->IsFly()) {
			auto blockPos = chunk->FindLocalBlock(block->getPosition());
			auto blockUnder = chunk->GetBlock(blockPos.x + 1, blockPos.y + 1);
			auto currentBlock = chunk->GetBlock(blockPos.x + 1, blockPos.y);
			if (blockUnder->getId() != 0 && !blockUnder->IsFly()) {
				block->SetFly(false);


				FlyingBlock* selectObj = nullptr;
				int selectInd = 0;

				for (int i = 0; i < Settings::world->getFlyingObjectsListPtr()->size(); i++) {
					selectObj = Settings::world->getFlyingObjectsListPtr()->at(i);


					if (selectObj->getBlock() == block) {
						selectInd = i;
						break;
					}
				}

				currentBlock->SetData(selectObj->getBlockData());
				chunk->getInteractibleBlockListPtr()->erase(chunk->getInteractibleBlockListPtr()->begin() + ind);

				chunk->getInteractibleBlockListPtr()->push_back(currentBlock);

				auto list = Settings::world->getFlyingObjectsListPtr();

				std::unique_lock<std::mutex> lock(Settings::world->w_mutex);
				Settings::world->condition.wait(lock);

				//if(Settings::world->getFlyingObjectsListPtr()->size()> selectBlockInd)

				list->erase(list->begin() + selectInd);

				lock.unlock();

				delete block;

				chunk->Change();
			}
			else {
				block->getTransform().Move(sf::Vector2f(0, 2));
			}

		}

		else if (chunk->GetBlock(block->getChunkPosition().x, block->getChunkPosition().y + 1)->getId() == 0 || chunk->GetBlock(block->getChunkPosition().x, block->getChunkPosition().y + 1)->IsFly()) {
			chunk->getInteractibleBlockListPtr()->at(ind) = new Block(block->GetData(), block->getTransform(), chunk);
			chunk->getInteractibleBlockListPtr()->at(ind)->SetFly(true);

			block->SetData(&World::blocks[0]);
			chunk->SetBlock(block->getChunkPosition().x, block->getChunkPosition().y, block->GetData());

			std::unique_lock<std::mutex> lock(Settings::world->w_mutex);
			Settings::world->condition.wait(lock);

			Settings::world->getFlyingObjectsListPtr()->push_back(new FlyingBlock(chunk->getInteractibleBlockListPtr()->at(ind), Settings::world->getFlyingObjectsListPtr()->size()));

			lock.unlock();

			chunk->Change();
		}
	}

	void OnDestroy(Block* block, Chunk* m_chunk) override {
		
	}

private:
	//FlyingBlock m_flyingBlock;
};