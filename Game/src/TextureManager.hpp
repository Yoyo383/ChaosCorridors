#pragma once
#include <unordered_map>
#include <string>
#include "SFML/Graphics.hpp"

class TextureManager
{
public:
	sf::Texture& operator[](std::string id);
	bool addTexture(std::string id, std::string filename);

private:
	std::unordered_map<std::string, sf::Texture> map;
};
