#pragma once

#include "globals.hpp"
#include "SFML/Graphics.hpp"

class Player {
public:
	Player();

	sf::Vector2f pos() const { return _pos; }
	sf::Vector2f velocity() const { return _velocity; }
	float direction() const { return _direction; }
	float fov() const { return _fov; }

	void setDirection(sf::RenderWindow& window, sf::Vector2i fixedMousePos);
	void calculateVelocity(sf::Vector2f wasd, float dt);
	void checkCollision(MazeArr& maze);
	void move();

private:
	sf::Vector2f _pos, _velocity;
	float _direction, _fov, _speed, _sensitivity;
};

