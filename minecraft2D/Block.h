#pragma once
#include"include.h"
#include"Transform.h"

struct BlockData {
	int id = 0;
	bool isPhisical = false;

	std::string name = "";
};



class Block {
public:
	Block(BlockData* data = nullptr, Transform transform = Transform()) {
		m_data = data;
		m_transform = transform;
	}

	inline void SetData(BlockData* data) {
		m_data = data;
	}
	inline BlockData* GetData() {
		return m_data;
	}

	inline int getId() {
		return m_data->id;
	}

	inline sf::Vector2f getPosition() {
		return m_transform.GetPosition();
	}

	inline Transform& getTransform() {
		return m_transform;
	}

	inline void setChunkPosition(sf::Vector2i pos) {
		m_chunkPosition = pos;
	}
	inline sf::Vector2i getChunkPosition() {
		return m_chunkPosition;
	}

	inline bool IsFly() {
		return m_isFly;
	}
	inline void SetFly(bool state) {
		m_isFly = state;
	}

private:
	BlockData* m_data;

	Transform m_transform;
	sf::Vector2i m_chunkPosition;

	bool m_isFly = false;


};