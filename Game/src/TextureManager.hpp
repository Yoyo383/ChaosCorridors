#pragma once
#include "Player.hpp"
#include <unordered_map>
#include <string>
#include "SFML/Graphics.hpp"

class TextureManager
{
private:
	std::unordered_map<std::string, sf::Texture> map;
public:
	sf::Texture& operator[](std::string id);
	bool addTexture(std::string id, std::string filename);
};
