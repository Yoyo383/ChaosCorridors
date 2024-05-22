#pragma once

#include "globals.hpp"
#include "util.hpp"
#include "SFML/System/Vector2.hpp"

struct Player
{
	Player(sf::Vector2f pos);
	Player(float x, float y);
	Player();

	void calculateVelocity(sf::Vector2f wasd, float dt);
	void checkCollision(const globals::MazeArr& maze);
	void move();


	inline static const float FOV = degToRad(60);
	inline static const float SPEED = 2.0f;
	inline static const float SENSITIVITY = 0.01f;

	sf::Vector2f pos, velocity;
	float direction;
	int lives;
};
