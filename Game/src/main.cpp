#include <math.h>
#include <iostream>
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "util.h"

/// <summary>
/// The function raycasts from pos to the mouse and finds a collision with the world.
/// It uses the DDA algorithm from this video: https://youtu.be/NbSee-XM7WA
/// </summary>
/// <param name="window">The window.</param>
/// <param name="pos">The starting position.</param>
/// <param name="world">The world.</param>
/// <returns>The coordinates of the collision. If there is no collision, it returns (-1, -1).</returns>
static sf::Vector2f raycast(sf::RenderWindow& window, sf::Vector2f pos, int world[][8]) {
	// the result
	sf::Vector2f hit(-1, -1);

	// get mouse position relative to screen
	sf::Vector2i mouseRaw = sf::Mouse::getPosition(window);
	sf::Vector2f mouse = sf::Vector2f((float)mouseRaw.x, (float)mouseRaw.y) / 100;

	// ray direction (also normalized)
	sf::Vector2f rayDir = vecNormalize(mouse - pos);

	// the unit step size
	sf::Vector2f rayUnitStepSize = { 
		sqrt(1 + (rayDir.y / rayDir.x) * (rayDir.y / rayDir.x)), 
		sqrt(1 + (rayDir.x / rayDir.y) * (rayDir.x / rayDir.y)) 
	};

	sf::Vector2i currentCell = { (int)pos.x, (int)pos.y };
	sf::Vector2f rayLength1D;
	sf::Vector2i step;

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
	float maxDistance = 8;
	float distance = 0;

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

		if (currentCell.x >= 0 && currentCell.x < 8 && currentCell.y >= 0 && currentCell.y < 8)
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
	sf::RenderWindow window(sf::VideoMode(800, 800), "Yay window!", sf::Style::Titlebar | sf::Style::Close);

	int world[][8] = {
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

	sf::Vector2f pos(4, 4);
	float speed = 4;

	sf::Vector2f hit(1, 6);

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

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				int value = world[i][j];
				sf::Color c;
				value == 0 ? c = sf::Color::White : c = sf::Color::Blue;
				sf::RectangleShape square(sf::Vector2f(100, 100));
				square.setFillColor(c);
				square.setPosition(j * 100, i * 100);
				window.draw(square);
			}
		}


		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			hit = raycast(window, pos, world);
			sf::Vector2i mouseRaw = sf::Mouse::getPosition(window);
			sf::Vertex line[] = { 
				sf::Vertex(pos * 100, sf::Color::Magenta), 
				sf::Vertex({(float)mouseRaw.x, (float)mouseRaw.y}, sf::Color::Magenta) 
			};
			window.draw(line, 2, sf::Lines);
		}
		else
			hit = { -1, -1 };


		circle.setPosition(pos * 100);
		window.draw(circle);

		if (hit != sf::Vector2f(-1, -1)) {
			sf::CircleShape collision(5);
			collision.setFillColor(sf::Color::Red);
			collision.setOrigin(5, 5);
			collision.setPosition(hit * 100);
			window.draw(collision);
		}

		window.display();
	}
}
