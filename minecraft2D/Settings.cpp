#include"include.h"

int Settings::seed = 1;

int Settings::worldSize = 10;
int Settings::threadCount = 2;
int Settings::fixedUpdateTime = 10;
int Settings::physicUpdateTime = 10;

float Settings::gravy = 0.5f;

sf::Vector2f Settings::blockSize = { 80,80 };
sf::Vector2f Settings::textureResolution = {128,128 };
sf::Vector2i Settings::atlasSize = { 16,16 };

sf::Vector2f Settings::windowSize = { 800,600 };

World* Settings::world = nullptr;
sf::RenderWindow* Settings::window = nullptr;
Program* Settings::program = nullptr;
ThreadManager* Settings::threadManager = nullptr;

std::map<std::string, int> Settings::collisionIds = {};
int Settings::endColId = 0;

int Settings::GetColId(std::string name) {
	return collisionIds[name];
}

void Settings::AddCol(std::string name) {
	collisionIds[name] = endColId;
	endColId++;
}


float toGlPositionX(float x) {
	return x/160;
}
float toGlPositionY(float y) {
	return y/160;
}
b2Vec2 toGlPosition(sf::Vector2f pos) {
	return { toGlPositionX(pos.x), toGlPositionY(pos.y) };
}

float toSfPositionX(float x) {
	return x*160;
}
float toSfPositionY(float y) {
	return y*160;
}
sf::Vector2f toSfPosition(b2Vec2 pos) {
	return { toSfPositionX(pos.x), toSfPositionY(pos.y) };
}

std::string getNowTimeAsString(std::string format) {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), format.c_str());

	return ss.str();
}