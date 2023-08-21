#pragma once
#include"include.h"
#include"Transform.h"
#include"CollisionEvents.hpp"
#include"Inventory.h"

class Player : public ICollisionEnterEvent {
public:
	Player(sf::Vector2f position) {
		m_transform = Transform();


		m_transform.SetPostion(position);
	}

	void SetInventory(Inventory* inventory) {
		m_inventory = inventory;
	}

	void InitInWorld();
	void Update();

	void SaveData();
	void LoadData();

	void UpdatePos(sf::Vector2f newPos) {
		m_transform.SetPostion(newPos);
	}

	void FixedUpdate();
	void Draw(sf::RenderWindow* window);

	void OnCollisionEnter(CollisionData data) override {
		if (data.collisionBId == Settings::GetColId("blockUpEdge") && data.contact->GetManifold()->localNormal ==b2Vec2 {0,-1}) {
			m_onGround = true;
		}	
	}
	void ExitFromGround() {
		//m_onGround = false;
	}

	sf::Vector2f getPosition() {
		return m_transform.GetPosition();
	}


	Transform* getTransform() {
		return &m_transform;
	}
private:
	Transform m_transform;

	Inventory* m_inventory;

	bool m_onGround = false;

	sf::Texture m_tx;
	sf::Sprite m_sp;

	int m_veiwDirection = 1;

	b2BodyDef m_playerLegBodyDef, m_playerBodyDef;
	b2Body* m_playerLegBody, *m_playerBody;
	b2PolygonShape m_playerLegBox, m_playerBox;
	b2FixtureDef m_playerLegFixtureDef, m_playerFixtureDef;

	b2WeldJointDef m_legWithBodyJointDef;
	b2WeldJoint* m_legWithBodyJoint;

};