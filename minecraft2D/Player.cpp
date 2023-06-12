#include"Player.hpp";
#include"World.h"

void Player::InitInWorld() {
	m_tx.loadFromFile("player.png");
	m_sp.setTexture(m_tx);

	m_sp.setOrigin(sf::Vector2f(m_sp.getLocalBounds().width / 2, 0));

	//creat body
	m_playerBodyDef.type = b2_dynamicBody;
	m_playerBodyDef.position.Set(toGlPositionX(-m_transform.GetPosition().x + 20),-1);
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
	//m_transform.SetPostion({-m_playerBody->GetPosition().x,- m_playerBody->GetPosition().y});
	b2Vec2 playerVelocity = m_playerBody->GetLinearVelocity();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		m_playerBody->SetLinearVelocity(b2Vec2(-0.6f, playerVelocity.y));
		m_veiwDirection = -1;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		m_playerBody->SetLinearVelocity(b2Vec2(0.6f, playerVelocity.y));
		m_veiwDirection = 1;
	}
	else if (playerVelocity.x != 0) {
		m_playerBody->SetLinearVelocity(b2Vec2(0, playerVelocity.y));
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && m_onGround && playerVelocity.y>=0.f) {
		m_playerBody->ApplyLinearImpulseToCenter({ 0,-1.2f }, true);
		m_onGround = false;
	}

}

void Player::Draw(sf::RenderWindow* window) {
	sf::Vector2f newPos = toSfPosition(m_playerBody->GetPosition());
	newPos.x *= -1;
	newPos.y *= -1;

	newPos += Settings::windowSize * 0.5f;

	m_transform.SetPostion(newPos);

	m_sp.setScale({(float) m_veiwDirection,1.f });

	window->draw(m_sp);
}