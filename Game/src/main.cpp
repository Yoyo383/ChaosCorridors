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

	if (foundCell)
		hit = pos + distance * rayDir;

	return hit;
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
	float speed = WORLD_SIZE / 4.0f;

	sf::Vector2f hit;

	sf::CircleShape circle(10);
	circle.setFillColor(sf::Color::Green);
	circle.setOrigin(10, 10);
	circle.setPosition(pos);

	while (window.isOpen()) {
		dt = deltaClock.restart().asSeconds();
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			window.close();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			pos.y -= speed * dt;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			pos.y += speed * dt;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			pos.x -= speed * dt;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			pos.x += speed * dt;

		window.clear(sf::Color::Black);

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


		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			hit = raycast(window, pos, world);
			sf::Vector2i mouseRaw = sf::Mouse::getPosition(window);
			sf::Vertex line[] = { 
				sf::Vertex(pos * CELL_SIZE, sf::Color::Magenta), 
				sf::Vertex({(float)mouseRaw.x, (float)mouseRaw.y}, sf::Color::Magenta) 
			};
			window.draw(line, 2, sf::Lines);
		}
		else
			hit = { -1, -1 };


		circle.setPosition(pos * CELL_SIZE);
		window.draw(circle);

		if (hit != sf::Vector2f(-1, -1)) {
			sf::CircleShape collision(5);
			collision.setFillColor(sf::Color::Red);
			collision.setOrigin(5, 5);
			collision.setPosition(hit * CELL_SIZE);
			window.draw(collision);
		}

		window.display();
	}
}
