#pragma once
#include"Player.hpp"
#include"Command.hpp"

class UpdatePosCommand : public ICommand {
public:
	UpdatePosCommand(Player* player, sf::Vector2f pos) {
		m_player = player;
		m_pos = pos;
	}

	void Excute() override {
		m_player->UpdatePos(m_pos);
	}
private:
	Player* m_player;
	sf::Vector2f m_pos;
};