#pragma once
#include <unordered_map>
#include <string>
#include "SFML/Graphics.hpp"

/**
 * @brief Class for managing textures with IDs.
 */
class TextureManager
{
public:
	/**
	 * @brief Gets a texture with an ID.
	 * @param id The ID of the texture.
	 * @return The texture with the ID. If no texture exists with this ID, throws an exception.
	 */
	sf::Texture& operator[](std::string id);

	/**
	 * @brief Adds a texture to the manager.
	 * @param id The ID of the new texture.
	 * @param filename The filename of the new texture.
	 * @return Whether the texture was added or not.
	 */
	bool addTexture(std::string id, std::string filename);

private:
	std::unordered_map<std::string, sf::Texture> map;
};
