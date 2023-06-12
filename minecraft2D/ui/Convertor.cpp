#include"Convertor.h"

sf::Color ColorFromString(std::string str) {
	std::string r, g, b, a;
	int load = 0;
	for (auto i : str) {
		if (i == '(' || i == ')') { continue; }
		else if (i == ';') { load++; }
		else {
			switch (load)
			{
			case 0:
				r += i;
				break;
			case 1:
				g += i;
				break;
			case 2:
				b += i;
				break;
			case 3:
				a += i;
				break;
			default:
				break;
			}
		}
	}

	return sf::Color(std::stof(r), std::stof(g), std::stof(b), a != "" ? std::stof(a) : 255);
}

sf::Vector2f Vec2FromString(std::string str) {
	std::string x, y;
	int load = 0;
	for (auto i : str) {
		if (i == '(' || i == ')') { continue; }
		else if (i == ';') { load++; }
		else {
			switch (load)
			{
			case 0:
				x += i;
				break;
			case 1:
				y += i;
				break;
			default:
				break;
			}
		}
	}

	return sf::Vector2f(std::stof(x), std::stof(y));
}