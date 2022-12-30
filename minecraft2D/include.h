#pragma once
#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<vector>
#include<iostream>
#include<map>
#include <fstream>
#include <string>
#include<thread>
#include<mutex>

class World;

const int chunkSizeX = 16;
const int chunkSizeY = 128;

class Settings {
public:
	static int worldSize;

	static int threadCount;
	static int fixedUpdateTime;

	static int seed;

	static int persistence;
	static int frequency;
	static int amplitude;
	static int octaves;

	static sf::Vector2f blockSize;
	static sf::Vector2f textureResolution ;
	static sf::Vector2i atlasSize;

	static sf::Vector2f windowSize;

	static World* world;


};
