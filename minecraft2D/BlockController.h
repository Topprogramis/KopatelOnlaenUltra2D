#pragma once
#include"include.h"
#include"Chunk.hpp"
#include"Inventory.h"

class BlockController {
public:
	BlockController(Inventory* inventory);


	void Draw();
	void FixedUpdate();


	Inventory* playerInventory;
private:
	Block* m_breakingBlock = nullptr;
	sf::RectangleShape m_breakingBlockShape;


	sf::Texture m_breakAnim;

	bool m_IsDownRight = false;
	bool m_IsDownLeft = false;

	Block* m_selectBlock = nullptr;

	void DestroyBlock(Block* currentBl);
	void UnSelectBlock(Block* currentBl);
};