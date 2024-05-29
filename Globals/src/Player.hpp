#pragma once

#include "globals.hpp"
#include "util.hpp"
#include "SFML/System/Vector2.hpp"

struct Player
{
	/**
	 * @brief Creates a new Player object with a position.
	 * @param pos The player position.
	 */
	Player(sf::Vector2f pos);

	/**
	 * @brief Calculates the velocity of the player.
	 * @param wasd The WASD input.
	 * @param dt Delta time.
	 */
	void calculateVelocity(sf::Vector2f wasd, float dt);

	/**
	 * @brief Checks for collision with the maze and sets the velocity accordingly.
	 * @param maze The maze.
	 */
	void checkCollision(const globals::MazeArr& maze);

	/**
	 * @brief Moves the player.
	 */
	void move();


	inline static const float FOV = degToRad(60);
	inline static const float SPEED = 2.0f;
	inline static const float SENSITIVITY = 0.005f;


	sf::Vector2f pos;
	sf::Vector2f velocity;
	float direction;
	int lives;
};
