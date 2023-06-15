#pragma once
#include"Command.hpp"

class GuiManager;

class SwapGuiTexture : public ICommand {
public:
	SwapGuiTexture(GuiManager* manager, sf::Texture* tx) {
		m_manager = manager;
		m_tx = tx;
	}

	void Excute() override;
private:
	GuiManager* m_manager;
	sf::Texture* m_tx;
};