#include "player.hpp"
#include "util.hpp"

Player::Player() {
	pos = { 1.5, 1.5 };
	direction = 0;
	fov = degToRad(60);
	speed = 2.0f;
	sensitivity = 0.01f;
}

void Player::setDirection(sf::RenderWindow& window, sf::Vector2i fixedMousePos, float dt) {
	/*int currentMousePos = sf::Mouse::getPosition(window).x;
	float deltaMousePos = (currentMousePos - fixedMousePos.x) * sensitivity;
	direction += deltaMousePos;*/
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		direction -= 2 * dt;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		direction += 2 * dt;
}

void Player::calculateVelocity(sf::Vector2f wasd, float dt) {
	// if there isn't input then velocity is 0
	if (wasd != sf::Vector2f()) {
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
	velocity = vecNormalize(velocity) * speed * dt;
}

void Player::checkCollision(globals::MazeArr& maze)
{
	// return sign of a number (1, -1, or 0)
	auto sign = [](float x) { return (x > 0) - (x < 0); };

	// move collision away from camera
	sf::Vector2f collisionRadius = sf::Vector2f(sign(velocity.x), sign(velocity.y)) * 0.25f;

	// checking collision
	if (maze[(int)(pos.y + velocity.y + collisionRadius.y)][(int)pos.x] == globals::CELL_WALL)
		velocity.y = 0;
	if (maze[(int)pos.y][(int)(pos.x + velocity.x + collisionRadius.x)] == globals::CELL_WALL)
		velocity.x = 0;
}

bool Player::move() {
	pos += velocity;
	return velocity.x != 0 || velocity.y != 0;
}
