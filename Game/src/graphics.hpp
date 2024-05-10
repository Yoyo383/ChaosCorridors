#pragma once
#include "player.hpp"
#include <unordered_map>

struct Ray {
	bool isHit;
	bool verticalHit;
	float distance;
	float hitCoord;
};

class TextureManager {
private:
	std::unordered_map<std::string, sf::Texture> map;
public:
	sf::Texture& operator[](std::string id);
	bool addTexture(std::string id, std::string filename);
};

/**
	* @brief The function raycasts from pos in a certain direction and finds a collision with the world.
	* It uses the DDA algorithm from this video: https://youtu.be/NbSee-XM7WA
	* @param pos The starting position.
	* @param angle The direction of the ray.
	* @param world The world.
	* @return A Ray object representing the ray.
*/
Ray raycast(sf::Vector2f pos, float angle, const globals::MazeArr& world);
