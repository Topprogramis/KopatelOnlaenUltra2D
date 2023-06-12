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
#include <future>

#include<Box2D.h>

#include<pugixml.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define _USE_MATH_DEFINES
#include <cmath>

#include"ui/Math.hpp"

class World;
class Program;

class Ray;

class RectTransform;
class UiElement;

class Button;

class Canvas;

const int chunkSizeX = 16;
const int chunkSizeY = 128;

class Settings {
public:
	static int worldSize;
	static float gravy;

	static int threadCount;
	static int fixedUpdateTime;
	static int physicUpdateTime;

	static int seed;

	static int persistence;
	static int frequency;
	static int amplitude;
	static int octaves;

	static sf::Vector2f blockSize;
	static sf::Vector2f textureResolution ;
	static sf::Vector2i atlasSize;

	static sf::Vector2f windowSize;

	static int GetColId(std::string name);
	static void AddCol(std::string name);

	static World* world;
	static sf::RenderWindow* window;
	static Program* program;
private:
	static std::map<std::string, int> collisionIds;
	static int endColId;
};

float toGlPositionX(float x);
float toGlPositionY(float y);
b2Vec2 toGlPosition(sf::Vector2f pos);

float toSfPositionX(float x);
float toSfPositionY(float y);
sf::Vector2f toSfPosition(b2Vec2 pos);


