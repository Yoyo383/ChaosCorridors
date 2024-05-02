#include "player.hpp"
#include "util.hpp"

Player::Player() {
	_pos = { 1.5, 1.5 };
	_direction = 0;
	_fov = degToRad(60);
	_speed = 2.0f;
	_sensitivity = 0.01f;
}

void Player::setDirection(sf::RenderWindow& window, sf::Vector2i fixedMousePos) {
	int currentMousePos = sf::Mouse::getPosition(window).x;
	float deltaMousePos = (currentMousePos - fixedMousePos.x) * _sensitivity;
	_direction += deltaMousePos;
}

void Player::calculateVelocity(sf::Vector2f wasd, float dt) {
	// if there isn't input then velocity is 0
	if (wasd != sf::Vector2f()) {
		float movementAngle = vecAngle(wasd);
		float cos = cosf(movementAngle), sin = sinf(movementAngle);

		sf::Vector2f dirVector = { cosf(_direction), sinf(_direction) };
		// rotating dirVector by movementAngle
		_velocity = {
			dirVector.x * cos - dirVector.y * sin,
			dirVector.x * sin + dirVector.y * cos
		};
	}
	else
		_velocity = { 0, 0 };

	// calculating velocity
	_velocity = vecNormalize(_velocity) * _speed * dt;
}

void Player::checkCollision(MazeArr& maze)
{
	// return sign of a number (1, -1, or 0)
	auto sign = [](float x) { return (x > 0) - (x < 0); };

	// move collision away from camera
	sf::Vector2f collisionRadius = sf::Vector2f(sign(_velocity.x), sign(_velocity.y)) * 0.25f;

	// checking collision
	if (maze[(int)(_pos.y + _velocity.y + collisionRadius.y)][(int)_pos.x] == consts::CELL_WALL)
		_velocity.y = 0;
	if (maze[(int)_pos.y][(int)(_pos.x + _velocity.x + collisionRadius.x)] == consts::CELL_WALL)
		_velocity.x = 0;
}

void Player::move() {
	_pos += _velocity;
}
