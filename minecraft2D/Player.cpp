#include"Player.hpp";
#include"World.h"
#include"ThreadManagaer.hpp"
#include"PlayerComands.hpp"

void Player::InitInWorld() {
	m_tx.loadFromFile("player.png");
	m_sp.setTexture(m_tx);

	m_sp.setOrigin(sf::Vector2f(m_sp.getLocalBounds().width / 2, 0));

	LoadData();

	//creat body
	m_playerBodyDef.type = b2_dynamicBody;
	m_playerBodyDef.position.Set(toGlPositionX(-m_transform.GetPosition().x + 20), toGlPositionY(m_transform.GetPosition().y));
	m_playerBodyDef.fixedRotation = true;
	m_playerBody = Settings::world->creatBody(&m_playerBodyDef);

	auto data = new CollisionData{ Settings::GetColId("playerBody"),"Player",this };
	data->enterE = this;

	m_playerBody->SetUserData(data);

	m_playerBox.SetAsBox(toGlPositionX(m_sp.getLocalBounds().width/2), toGlPositionY(m_sp.getLocalBounds().height/2), toGlPosition({ m_sp.getLocalBounds().width / 2, m_sp.getLocalBounds().height / 2  }), 0);


	m_playerFixtureDef.shape = &m_playerBox;
	m_playerFixtureDef.density = 1.0f;
	m_playerFixtureDef.friction = 0.0f;

	m_playerBody->CreateFixture(&m_playerFixtureDef);

	//creat leg
	m_playerLegBodyDef.type = b2_dynamicBody;
	m_playerLegBodyDef.position.Set(toGlPositionX(-m_transform.GetPosition().x + 20), -1);
	m_playerLegBodyDef.fixedRotation = true;
	m_playerLegBody = Settings::world->creatBody(&m_playerLegBodyDef);

	m_playerLegBody->SetUserData(new CollisionData{ Settings::GetColId("playerLeg"),"Player",this });

	//m_playerLegBox.SetAsBox(toGlPositionX(m_sp.getLocalBounds().width / 2), toGlPositionY(30), toGlPosition({ m_sp.getLocalBounds().width / 2, m_sp.getLocalBounds().height / 2 +20}), 0);


	m_playerLegFixtureDef.shape = &m_playerLegBox;
	m_playerLegFixtureDef.density = 1.0f;
	m_playerLegFixtureDef.friction = 0.0f;

	//m_playerLegBody->CreateFixture(&m_playerFixtureDef);
	
	//creat joint
	m_legWithBodyJointDef.bodyA = m_playerBody;
	m_legWithBodyJointDef.bodyB = m_playerLegBody;
	m_legWithBodyJointDef.collideConnected = true;

	//m_legWithBodyJoint = Settings::world->creatJoint(&m_legWithBodyJointDef);


	m_sp.setPosition(Settings::windowSize * 0.5f + sf::Vector2f(m_sp.getLocalBounds().width / 2, 0));
}


void Player::FixedUpdate() {
	sf::Vector2f newPos = toSfPosition(m_playerBody->GetPosition());
	newPos.x *= -1;
	newPos.y *= -1;

	newPos += Settings::windowSize * 0.5f;

	Settings::threadManager->TryAddCommand("main", new UpdatePosCommand(this, newPos));
	b2Vec2 playerVelocity = m_playerBody->GetLinearVelocity();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		m_playerBody->SetLinearVelocity(b2Vec2(-30.f * (Settings::physicUpdateTime/1000.f), playerVelocity.y));
		m_veiwDirection = -1;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		m_playerBody->SetLinearVelocity(b2Vec2(30.f * (Settings::physicUpdateTime /1000.f), playerVelocity.y));
		m_veiwDirection = 1;
	}
	else if (playerVelocity.x != 0) {
		m_playerBody->SetLinearVelocity(b2Vec2(0, playerVelocity.y));
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && m_onGround && playerVelocity.y>=0.f) {
		m_playerBody->ApplyLinearImpulseToCenter({ 0,-8.f * (Settings::physicUpdateTime / 1000.f) }, true);
		m_onGround = false;
	}

}

void Player::Draw(sf::RenderWindow* window) {
	m_sp.setScale({(float) m_veiwDirection,1.f });

	window->draw(m_sp);
}

void Player::SaveData() {
	std::ofstream saveFile;

	saveFile.open("saves\\player.txt");
	if (!saveFile.is_open()) {
		std::cout << "Save is no open. Please create in game files saves/player.txt" << std::endl;
		return;
	}

	saveFile.clear();

	std::string position = "posX:" + std::to_string(toSfPositionX(m_playerBody->GetPosition().x)) + ";posY:" + std::to_string(toSfPositionY(m_playerBody->GetPosition().y)) +";\n";

	saveFile << position;
	saveFile << m_inventory->Serialize();

	
	saveFile.close();

	std::cout << "player data saves" << std::endl;

}

void Player::LoadData()  {
	std::ifstream saveFile;

	saveFile.open("saves\\player.txt");
	if (!saveFile.is_open()) {
		std::cout << "Save is not found. Please create in game files saves/player.txt" << std::endl;
		return;
	}

	sf::Vector2f pos;


	std::string buf;

	std::getline(saveFile, buf);
	bool initName = false;

	std::string x, y;

	std::string name = "";
	std::string data = "";
	for (auto& i : buf) {
		if (i == ':') {
			initName = true;
			continue;
		}
		else if (i == ';') {
			if (name == "posX") {
				x = data;

				initName = false;
				name = "";
				data = "";
			}
			if (name == "posY") {
				y = data;

				initName = false;
				name = "";
				data = "";
			}
		}
		else if (initName) {
			data += i;
		}
		else {
			name += i;
		}
	}

	if(x!="" && y != "")
		m_transform.SetPostion({ -std::stof(x),std::stof(y)-160 });

	saveFile.close();

}