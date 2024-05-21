#include "TextureManager.hpp"
#include "util.hpp"
#include <iostream>

bool TextureManager::addTexture(std::string id, std::string filename)
{
	if (map.find(id) != map.end())
	{
		std::cout << "Texture with the ID of '" << id << "' already exists." << std::endl;;
		return false;
	}

	sf::Texture texture;
	if (!texture.loadFromFile(filename))
		throw std::exception(("ERROR: Can't load " + filename).c_str());

	map[id] = texture;
	return true;
}

sf::Texture& TextureManager::operator[](std::string id)
{
	if (map.find(id) == map.end())
		throw std::exception(("ERROR: No texture with the ID of '" + id + "'.").c_str());
	return map[id];
}
