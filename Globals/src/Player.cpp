#include "Player.hpp"
#include "util.hpp"

Player::Player(sf::Vector2f pos) : pos(pos), direction(0), velocity(0, 0) {}
Player::Player() : pos(1.5f, 1.5f), direction(0), velocity(0, 0) {}

void Player::calculateVelocity(sf::Vector2f wasd, float dt)
{
	// if there isn't input then velocity is 0
	if (wasd != sf::Vector2f())
	{
		float movementAngle = vecAngle(wasd);
		float cos = cosf(movementAngle), sin = sinf(movementAngle);

		sf::Vector2f dirVector = { cosf(direction), sinf(direction) };
		// rotating dirVector by movementAngle
		velocity = {
			dirVector.x * cos - dirVector.y * sin,
			dirVector.x * sin + dirVector.y * cos
		};
	}
	else
		velocity = { 0, 0 };

	// calculating velocity
	velocity = vecNormalize(velocity) * SPEED * dt;
}

void Player::checkCollision(const globals::MazeArr& maze)
{
	float radius = 0.25f;

	// return sign of a number (1, -1, or 0)
	auto sign = [](float x) { return (x > 0) - (x < 0); };

	// move collision away from camera
	sf::Vector2f collisionRadius = sf::Vector2f(sign(velocity.x), sign(velocity.y)) * radius;

	// checking collision
	if (maze[(int)(pos.y + velocity.y + collisionRadius.y)][(int)pos.x] == globals::CELL_WALL)
		velocity.y = 0;
	if (maze[(int)pos.y][(int)(pos.x + velocity.x + collisionRadius.x)] == globals::CELL_WALL)
		velocity.x = 0;
}

void Player::move()
{
	pos += velocity;
}
