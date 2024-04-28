#pragma once
#include "player.hpp"

namespace graphics {

	struct Ray {
		bool isHit;
		bool verticalHit;
		float distance;
		float hitCoord;
	};

	struct Textures {
		sf::Texture wallTexture, floorTexture, ceilingTexture, characterTexture;
	};

	/**
	 * @brief The function raycasts from pos in a certain direction and finds a collision with the world.
	 * It uses the DDA algorithm from this video: https://youtu.be/NbSee-XM7WA
	 * @param pos The starting position.
	 * @param angle The direction of the ray.
	 * @param world The world.
	 * @return A Ray object representing the ray.
	*/
	Ray raycast(sf::Vector2f pos, float angle, const MazeArr& world);

	Textures loadTextures();

	void drawFloorAndCeiling(sf::RenderWindow& window, const Player& player, Textures textures);
	void drawWalls(sf::RenderWindow& window, const Player& player, const MazeArr& maze, float* zBuffer, Textures textures);
	void drawCharacter(sf::RenderWindow& window, const Player& player, const sf::Vector2f& characterPos, float* zBuffer, Textures textures);

}
