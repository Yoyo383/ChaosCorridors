#include <math.h>
#include <iostream>
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "util.h"

const int WINDOW_SIZE = 800;
const int WORLD_SIZE = 8;
const int CELL_SIZE = WINDOW_SIZE / WORLD_SIZE;

/// <summary>
/// The function raycasts from pos in a certain direction and finds a collision with the world.
/// It uses the DDA algorithm from this video: https://youtu.be/NbSee-XM7WA
/// </summary>
/// <param name="pos">The starting position.</param>
/// <param name="angle">The direction of the ray.</param>
/// <param name="world">The world.</param>
/// <returns>Whether the ray hit + the distance of the ray. If there is no collision, it returns (-1, -1).</returns>
static std::pair<bool, float> raycast(sf::Vector2f pos, float angle, int world[][WORLD_SIZE]) {
	// the result
	sf::Vector2f hit(-1, -1);

	// ray direction
	sf::Vector2f rayDir = { cosf(angle), sinf(angle) };

	// the unit step size
	sf::Vector2f rayUnitStepSize = { 
		sqrt(1 + (rayDir.y / rayDir.x) * (rayDir.y / rayDir.x)), 
		sqrt(1 + (rayDir.x / rayDir.y) * (rayDir.x / rayDir.y)) 
	};

	sf::Vector2i currentCell = { (int)pos.x, (int)pos.y };
	sf::Vector2f rayLength1D;
	sf::Vector2i step;

	// set step and initial ray length
	if (rayDir.x < 0) {
		step.x = -1;
		rayLength1D.x = (pos.x - float(currentCell.x)) * rayUnitStepSize.x;
	}
	else {
		step.x = 1;
		rayLength1D.x = (float(currentCell.x + 1) - pos.x) * rayUnitStepSize.x;
	}

	if (rayDir.y < 0) {
		step.y = -1;
		rayLength1D.y = (pos.y - float(currentCell.y)) * rayUnitStepSize.y;
	}
	else {
		step.y = 1;
		rayLength1D.y = (float(currentCell.y + 1) - pos.y) * rayUnitStepSize.y;
	}

	bool foundCell = false;
	float maxDistance = WORLD_SIZE;
	float distance = 0;

	// walk on the ray until collision (or distance is bigger than maxDistance)
	while (!foundCell && distance < maxDistance) {
		if (rayLength1D.x < rayLength1D.y) {
			currentCell.x += step.x;
			distance = rayLength1D.x;
			rayLength1D.x += rayUnitStepSize.x;
		}
		else {
			currentCell.y += step.y;
			distance = rayLength1D.y;
			rayLength1D.y += rayUnitStepSize.y;
		}

		if (currentCell.x >= 0 && currentCell.x < WORLD_SIZE && currentCell.y >= 0 && currentCell.y < WORLD_SIZE)
		{
			if (world[currentCell.y][currentCell.x] == 1)
			{
				foundCell = true;
			}
		}
	}

	return std::make_pair(foundCell, distance);
}

static sf::Vector2f wasdInput() {
	return {
		(float)(sf::Keyboard::isKeyPressed(sf::Keyboard::W) - sf::Keyboard::isKeyPressed(sf::Keyboard::S)),
		(float)(sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	};
}

int main() {
	sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Yay window!", sf::Style::Titlebar | sf::Style::Close);

	int world[][WORLD_SIZE] = {
		{1, 1, 1, 1, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 1, 0, 1, 0, 1, 1},
		{1, 1, 1, 0, 1, 0, 0, 1},
		{1, 0, 0, 0, 1, 0, 0, 1},
		{1, 0, 1, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1}
	};

	sf::Clock deltaClock;
	float dt;

	sf::Vector2f pos(WORLD_SIZE / 2, WORLD_SIZE / 2);
	sf::Vector2f velocity;
	float direction = 0;
	float fov = degToRad(70);
	float speed = WORLD_SIZE / 4.0f;

	sf::Vector2f hit;

	sf::CircleShape playerCircle(10);
	playerCircle.setFillColor(sf::Color::Green);
	playerCircle.setOrigin(10, 10);
	playerCircle.setPosition(pos);

	while (window.isOpen()) {
		dt = deltaClock.restart().asSeconds();
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed)
				if (event.key.code == sf::Keyboard::Escape)
					window.close();
		}

		// setting player's direction
		direction = vecAngle(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)) - sf::Vector2f(WINDOW_SIZE / 2, WINDOW_SIZE / 2));

		// getting input
		sf::Vector2f wasd = wasdInput();
		if (wasd != sf::Vector2f()) {
			float movementAngle = vecAngle(wasd);
			float cos = cosf(movementAngle), sin = sinf(movementAngle);
			// fix weird bug
			if (abs(cos) < 0.00001f)
				cos = 0;
			if (abs(sin) < 0.00001f)
				sin = 0;

			sf::Vector2f dirVector = { cosf(direction), sinf(direction) };
			// rotating dirVector by movementAngle
			velocity = {
				dirVector.x * cos - dirVector.y * sin,
				dirVector.x * sin + dirVector.y * cos
			};
		}
		else
			velocity = { 0, 0 };
		
		// applying velocity
		velocity = vecNormalize(velocity) * speed * dt;
		sf::Vector2f nextPos = pos + velocity;
		if (world[(int)nextPos.y][(int)nextPos.x] == 0)
			pos = nextPos;


		window.clear(sf::Color::Black);

		// drawing the world
		for (int i = 0; i < WORLD_SIZE; i++) {
			for (int j = 0; j < WORLD_SIZE; j++) {
				int value = world[i][j];
				sf::Color c;
				value == 0 ? c = sf::Color::White : c = sf::Color::Blue;
				sf::RectangleShape square(sf::Vector2f(CELL_SIZE, CELL_SIZE));
				square.setFillColor(c);
				square.setPosition(j * CELL_SIZE, i * CELL_SIZE);
				window.draw(square);
			}
		}

		playerCircle.setPosition(pos * CELL_SIZE);
		window.draw(playerCircle);

		window.display();
	}
}
