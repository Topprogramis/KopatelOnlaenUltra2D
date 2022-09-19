#include"include.h"

int Settings::worldSize = 10;
int Settings::threadCount = 2;
int Settings::fixedUpdateTime = 10;

sf::Vector2f Settings::Settings::blockSize = { 80,80 };
sf::Vector2f Settings::Settings::textureResolution = {128,128 };
sf::Vector2i Settings::Settings::atlasSize = { 16,16 };

World* Settings::world = nullptr;