#pragma once
#include"Command.hpp"
#include"Chunk.hpp"


class InteractBlockChenge : public ICommand {
public:
	InteractBlockChenge(Chunk* chunk) {
		m_chunk = chunk;
	}

	void Excute() override {
		m_chunk->ChangeInterBlock();
	}
private:
	Chunk* m_chunk;

};

class ChunkVertexSwapCommand : public ICommand {
public:
	ChunkVertexSwapCommand(Chunk* chunk, sf::VertexArray newVertexArray) {
		m_chunk = chunk;
		m_newVertexArray = newVertexArray;
	}

	void Excute() override {
		m_chunk->SwapVertexArray(m_newVertexArray);
	}
private:
	Chunk* m_chunk;
	sf::VertexArray m_newVertexArray;
	
};
class ChunkBuildChangeCommand : public ICommand {
public:
	ChunkBuildChangeCommand(Chunk* chunk, bool state) {
		m_chunk = chunk;
		m_state = state;
	}

	void Excute() override {
		m_chunk->ChangeInThisThread(m_state);
	}
private:
	Chunk* m_chunk;
	bool m_state;

};
class ChunkColChangeCommand : public ICommand {
public:
	ChunkColChangeCommand(Chunk* chunk, bool state) {
		m_chunk = chunk;
		m_state = state;
	}

	void Excute() override {
		m_chunk->ChangeCol(m_state);
	}
private:
	Chunk* m_chunk;
	bool m_state;

};