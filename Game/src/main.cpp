#include <math.h>
#include <iostream>
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "util.h"
#include "maze.h"

/// <summary>
/// The function raycasts from pos in a certain direction and finds a collision with the world.
/// It uses the DDA algorithm from this video: https://youtu.be/NbSee-XM7WA
/// </summary>
/// <param name="pos">The starting position.</param>
/// <param name="angle">The direction of the ray.</param>
/// <param name="world">The world.</param>
/// <returns>Whether the ray hit + the distance of the ray. If there is no collision, it returns (-1, -1).</returns>
static std::tuple<bool, float, bool> raycast(sf::Vector2f pos, float angle, MazeArr world) {
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
	bool differentColor = false;
	float maxDistance = consts::WORLD_WIDTH;
	float distance = 0;

	// walk on the ray until collision (or distance is bigger than maxDistance)
	while (!foundCell && distance < maxDistance) {
		if (rayLength1D.x < rayLength1D.y) {
			currentCell.x += step.x;
			distance = rayLength1D.x;
			rayLength1D.x += rayUnitStepSize.x;
			differentColor = true;
		}
		else {
			currentCell.y += step.y;
			distance = rayLength1D.y;
			rayLength1D.y += rayUnitStepSize.y;
			differentColor = false;
		}

		if (currentCell.x >= 0 && currentCell.x < consts::WORLD_WIDTH && currentCell.y >= 0 && currentCell.y < consts::WORLD_HEIGHT)
		{
			if (world[currentCell.y][currentCell.x] == consts::CELL_WALL)
			{
				foundCell = true;
			}
		}
	}

	return std::make_tuple(foundCell, distance, differentColor);
}

static sf::Vector2f wasdInput() {
	return {
		(float)(sf::Keyboard::isKeyPressed(sf::Keyboard::W) - sf::Keyboard::isKeyPressed(sf::Keyboard::S)),
		(float)(sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	};
}

int main() {
	srand(time(NULL));

	sf::RenderWindow window(sf::VideoMode(1000, 800), "Yay window!");
	window.setMouseCursorVisible(false);

	MazeArr world = generateMaze();

	sf::Clock deltaClock;
	float dt;

	sf::Vector2f pos(1.5, 1.5);
	sf::Vector2f velocity;
	float direction = 0;
	float fov = degToRad(70);
	float speed = 2.0f;

	float sensitivity = 1.3;

	sf::Vector2i fixedMousePos = { (int)window.getSize().x / 2, (int)window.getSize().y / 2 };
	bool isFocused = true;

	sf::Mouse::setPosition(fixedMousePos, window);

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
			else if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape)
					window.close();
			}
			else if (event.type == sf::Event::LostFocus)
				isFocused = false;
			else if (event.type == sf::Event::GainedFocus)
				isFocused = true;
		}

		// setting player's direction according to mouse
		if (isFocused) {
			int currentMousePos = sf::Mouse::getPosition(window).x;
			float deltaMousePos = (currentMousePos - fixedMousePos.x) * sensitivity;
			direction += deltaMousePos * dt;
			// always setting the mouse position to the center
			sf::Mouse::setPosition(fixedMousePos, window);
		}

		// getting input
		sf::Vector2f wasd;
		sf::Vector2f dirVector;

		if (isFocused)
			wasd = wasdInput();
		if (wasd != sf::Vector2f()) {
			float movementAngle = vecAngle(wasd);
			float cos = cosf(movementAngle), sin = sinf(movementAngle);

			dirVector = { cosf(direction), sinf(direction) };
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

		// return sign of a number (1, -1, or 0)
		auto sign = [](float x) {
			return (x > 0) - (x < 0);
		};

		// move collision away from camera
		sf::Vector2f collisionRadius = sf::Vector2f(sign(velocity.x), sign(velocity.y)) * 0.25f;
		
		// checking collision
		if (world[(int)(pos.y + velocity.y + collisionRadius.y)][(int)pos.x] == consts::CELL_WALL)
			velocity.y = 0;
		if (world[(int)pos.y][(int)(pos.x + velocity.x + collisionRadius.x)] == consts::CELL_WALL)
			velocity.x = 0;

		pos += velocity;

		// start drawing

		window.clear(sf::Color::Black);

		sf::RectangleShape sky({ (float)window.getSize().x, (float)window.getSize().y / 2 });
		sky.setPosition({ 0, 0 });
		sky.setFillColor({ 135, 206, 235 });
		window.draw(sky);

		sf::RectangleShape ground({ (float)window.getSize().x, (float)window.getSize().y / 2 });
		ground.setPosition({ 0, (float)window.getSize().y / 2 });
		ground.setFillColor({ 38, 139, 7 });
		window.draw(ground);

		float angle;
		for (int x = 0; x <= window.getSize().x; x++) {
			angle = (direction - fov / 2.0f) + ((float)x / (float)window.getSize().x) * fov;

			// casting ray and fixing the fisheye problem
			auto[isHit, distance, differentColor] = raycast(pos, angle, world);
			distance *= cosf(direction - angle);

			if (!isHit)
				continue;

			float wallHeight = ((float)window.getSize().y) / (2 * distance);
			if (wallHeight > (float)window.getSize().y)
				wallHeight = (float)window.getSize().y;

			// calculating floor and ceiling y values
			float ceiling = (window.getSize().y / 2.0f) - (wallHeight / 2.0f);
			float floor = window.getSize().y - ceiling;

			// calculating shading
			sf::Color color(100, 100, 100);
			float brightness = 1.0f - (distance / 10);

			// darkening the walls with different color
			if (differentColor)
				brightness *= 0.7;

			if (brightness < 0.0f)
				brightness = 0.0f;

			// apply brightness
			color.r *= brightness;
			color.g *= brightness;
			color.b *= brightness;

			sf::Vertex blockLine[2] = {
				sf::Vertex({ (float)x, ceiling }, color),
				sf::Vertex({ (float)x, floor }, color)
			};

			window.draw(blockLine, 2, sf::Lines);
		}

		window.display();
	}
}
