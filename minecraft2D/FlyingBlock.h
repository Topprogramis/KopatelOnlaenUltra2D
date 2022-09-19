#pragma once
#include"Block.h"
#include"Chunk.hpp"

class FlyingBlock {
public:
	FlyingBlock() {


	}
	FlyingBlock(Block* block, int ind) {
		m_block = block;
		m_blockType = block->GetData();

		m_shape = sf::RectangleShape(block->getTransform().GetSize());
		m_shape.setTexture(block->getChunk()->getAtlas());
		m_shape.setTextureRect(sf::IntRect(Settings::textureResolution.x * (block->getId() % Settings::atlasSize.x), Settings::textureResolution.y * std::floor(block->getId() / Settings::atlasSize.y), Settings::textureResolution.x, Settings::textureResolution.y));
	
		m_ind = ind;
	}

	bool operator==(const FlyingBlock& f1){
		return f1.m_ind==m_ind;
	}

	void Update() {
		m_shape.setTextureRect(sf::IntRect(Settings::textureResolution.x * (m_blockType->id % Settings::atlasSize.x), Settings::textureResolution.y * std::floor(m_blockType->id / Settings::atlasSize.y), Settings::textureResolution.x, Settings::textureResolution.y));
	}

	Block* getBlock() {
		return m_block;
	}
	BlockData*& getBlockData() {
		return m_blockType;
	}

	sf::RectangleShape* getShapePtr() {
		return &m_shape;
	}
	int GetInd() {
		return m_ind;
	}

private:
	sf::RectangleShape m_shape;
	Block* m_block;
	BlockData* m_blockType;

	int m_ind = 0;
};