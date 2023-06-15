#include"BlockController.h"
#include"World.h"


BlockController::BlockController(Inventory* inventory) {
	playerInventory = inventory;

	m_breakAnim.loadFromFile("brokeAnim.png");

	m_breakingBlockShape.setSize(sf::Vector2f(Settings::blockSize.x, Settings::blockSize.y));
	m_breakingBlockShape.setTexture(&m_breakAnim);
}


void BlockController::FixedUpdate() {
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		m_selectBlock = Settings::world->findBlock((sf::Vector2f)sf::Mouse::getPosition(*Settings::window));

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			DestroyBlock(m_selectBlock);
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			if (Settings::world->buildBlock(m_selectBlock, &World::blocks[playerInventory->getSelect().id])) {
				playerInventory->PopItem(playerInventory->getSelect().id);
			}
			else if(m_IsDownRight){
				Settings::world->Interact(m_selectBlock);
			}
		}
	}
	else {
		if (m_selectBlock)
			UnSelectBlock(m_selectBlock);
	}

	if (!playerInventory->isOpen()) {
		m_IsDownRight = sf::Mouse::isButtonPressed(sf::Mouse::Right);
		m_IsDownLeft = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	}
}

void BlockController::Draw() {
	if (m_breakingBlock)
		Settings::window->draw(m_breakingBlockShape);
}

void BlockController::DestroyBlock(Block* currentBl) {
	if (currentBl == nullptr || currentBl->GetData() == nullptr) {
		m_breakingBlock = nullptr;
		return;
	}
	if (m_breakingBlock == nullptr)
		m_breakingBlock = currentBl;
	else if (m_breakingBlock != currentBl) {
		m_breakingBlock->ResetBreakStage();
		m_breakingBlock = currentBl;
	}

	if (currentBl->GetData()->id == 0) {
		m_breakingBlock->ResetBreakStage();
		m_breakingBlock = nullptr;
		return;
	}

	currentBl->AddBreakStage((float)Settings::fixedUpdateTime / 100.f);
	m_breakingBlockShape.setTextureRect(sf::IntRect(16 * (int)currentBl->GetBreakStage() - 1, 0, 16, 16));
	m_breakingBlockShape.setPosition(currentBl->getPosition() + currentBl->getChunk()->getPosition() + Settings::world->getPlayer()->getTransform()->GetPosition());

	if (currentBl->GetBreakStage() >= 10) {
		Settings::world->DestroyBlock(currentBl);
	}
}

void BlockController::UnSelectBlock(Block* currentBl) {
	if (m_breakingBlock == currentBl) {
		m_breakingBlock->ResetBreakStage();
		m_breakingBlock = nullptr;
	}
}