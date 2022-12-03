#include"include.h"

int Settings::seed = 1;

int Settings::persistence = 1;
int Settings::frequency = 1;
int Settings::amplitude = 1;
int Settings::octaves = 1;

int Settings::worldSize = 10;
int Settings::threadCount = 2;
int Settings::fixedUpdateTime = 10;

sf::Vector2f Settings::Settings::blockSize = { 80,80 };
sf::Vector2f Settings::Settings::textureResolution = {128,128 };
sf::Vector2i Settings::Settings::atlasSize = { 16,16 };

World* Settings::world = nullptr;