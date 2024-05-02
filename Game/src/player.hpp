#pragma once

#include "globals.hpp"
#include "SFML/Graphics.hpp"

class Player {
public:
	Player();

	sf::Vector2f getPos() const { return pos; }
	float getDirection() const { return direction; }
	float getFOV() const { return fov; }

	void setDirection(sf::RenderWindow& window, sf::Vector2i fixedMousePos);
	void calculateVelocity(sf::Vector2f wasd, float dt);
	void checkCollision(MazeArr& maze);
	void move();

private:
	sf::Vector2f pos, velocity;
	float direction, fov, speed, sensitivity;
};

