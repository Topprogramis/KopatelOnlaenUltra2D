#pragma once
#include"include.h"
#include"Transform.h"

class InteractibleComponent;
class Chunk;
class Block;

struct BlockData {
	int id = 0;
	bool isPhisical = false;

	std::string name = "";

	InteractibleComponent* component;
};

class InteractibleComponent {
public:
	virtual InteractibleComponent* copy() =0;

	virtual void Update(Block* block, Chunk* m_chunk, int blockInd) {

	}

	virtual void OnInteract(Block* block, Chunk* m_chunk) {

	}

	virtual void OnChange(Block* block, Chunk* m_chunk, int ind) {

	}


	virtual void OnDestroy(Block* block, Chunk* m_chunk) {

	}

	virtual void OnDown(Block* block, Chunk* m_chunk) {

	}
};

class Block {
public:
	Block(BlockData* data = nullptr, Transform transform = Transform(), Chunk* chunk = nullptr) {
		m_chunk = chunk;
		SetData(data);
		m_transform = transform;
	}
	void operator=(const Block& block){
		SetData(block.m_data);
		m_transform = block.m_transform;
		m_chunk = block.m_chunk;
		m_isFly = block.m_isFly;
	}

	inline Chunk* getChunk() {
		return m_chunk;
	}

	inline void AddBreakStage(float step) {
		m_breakStage += step;
	}
	inline float GetBreakStage() {
		return m_breakStage;
	}
	inline void ResetBreakStage() {
		m_breakStage = 0;
	}

	inline void SetData(BlockData* data) {	
		m_data = data;

		if (component != nullptr) {
			component->OnDestroy(this,m_chunk);
			delete component;
			component = nullptr;
		}

		if (data != nullptr && data->component) {		
			component = data->component->copy();
		}
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


	InteractibleComponent* component = nullptr;

private:
	BlockData* m_data;

	Transform m_transform;
	sf::Vector2i m_chunkPosition;

	Chunk* m_chunk;

	bool m_isFly = false;
	float m_breakStage = 0;

};


class FlyingBlock;