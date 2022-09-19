#pragma once
#include"Block.h"
#include"Chunk.hpp"

class MusicBlock;

class MusicBlock : public InteractibleComponent
{
public:
	MusicBlock(MusicBlock* block, std::vector<sf::Music*> music) {
		m_sounds = music;
	}

	MusicBlock() {
		int i = 0;
		while (1) {
			sf::Music* sound = new sf::Music();
			if (sound->openFromFile("audio/music/track" + std::to_string(i + 1) + ".ogg")) {
				m_sounds.push_back(sound);
			}
			else {
				break;
			}

			i++;
		}
	}

	InteractibleComponent* copy() override {
		return new MusicBlock(this,m_sounds);
	}

	void Update(Block* block, Chunk* chunk, int ind) override {
		
	}

	void OnDestroy(Block*, Chunk* chunk) {
		for (auto& i : m_sounds) {
			i->stop();
		}
	}

	void OnInteract(Block* block, Chunk* chunk) override {
		m_isPlaing = true;
		if(m_selectTrack==0)
			m_sounds[9]->stop();
		else {
			m_sounds[m_selectTrack - 1]->stop();
		}


		m_sounds[m_selectTrack]->play();

		m_selectTrack++;
		if (m_selectTrack == 10) {
			m_selectTrack = 0;
		}
	}

private:
	std::vector<sf::Music*> m_sounds;

	bool m_isPlaing = false;

	int m_selectTrack = 0;
};