#include <math.h>
#include <iostream>
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "util.hpp"
#include "maze.hpp"
#include "player.hpp"

struct Ray {
	bool isHit;
	bool verticalHit;
	float distance;
	float hitCoord;
};

/// <summary>
/// The function raycasts from pos in a certain direction and finds a collision with the world.
/// It uses the DDA algorithm from this video: https://youtu.be/NbSee-XM7WA
/// </summary>
/// <param name="pos">The starting position.</param>
/// <param name="angle">The direction of the ray.</param>
/// <param name="world">The world.</param>
/// <returns>A Ray object.</returns>
static Ray raycast(sf::Vector2f pos, float angle, MazeArr& world) {
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
	bool verticalHit = false;
	float maxDistance = consts::WORLD_WIDTH;
	float distance = 0;
	float hitCoord = 0;

	// walk on the ray until collision (or distance is bigger than maxDistance)
	while (!foundCell && distance < maxDistance) {
		if (rayLength1D.x < rayLength1D.y) {
			currentCell.x += step.x;
			distance = rayLength1D.x;
			rayLength1D.x += rayUnitStepSize.x;
			verticalHit = true;
		}
		else {
			currentCell.y += step.y;
			distance = rayLength1D.y;
			rayLength1D.y += rayUnitStepSize.y;
			verticalHit = false;
		}

		if (currentCell.x >= 0 && currentCell.x < consts::WORLD_WIDTH && currentCell.y >= 0 && currentCell.y < consts::WORLD_HEIGHT)
		{
			if (world[currentCell.y][currentCell.x] == consts::CELL_WALL)
			{
				foundCell = true;
			}
		}
	}

	sf::Vector2f hitPos = pos + rayDir * distance;

	if (verticalHit)
		hitCoord = hitPos.y;
	else
		hitCoord = hitPos.x;

	return { foundCell, verticalHit, distance, hitCoord - int(hitCoord) };
}

static sf::Vector2f wasdInput() {
	return {
		(float)(sf::Keyboard::isKeyPressed(sf::Keyboard::W) - sf::Keyboard::isKeyPressed(sf::Keyboard::S)),
		(float)(sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	};
}

static void resetMousePos(sf::RenderWindow& window, sf::Vector2i fixedMousePos) {
	sf::Mouse::setPosition(fixedMousePos, window);
}

int main() {
	srand(time(NULL));

	sf::RenderWindow window(sf::VideoMode(1000, 800), "Yay window!");
	window.setMouseCursorVisible(false);

	MazeArr maze = generateMaze();

	sf::Clock deltaClock;
	float dt;

	Player player;

	sf::Vector2i fixedMousePos = { (int)window.getSize().x / 2, (int)window.getSize().y / 2 };
	bool isFocused = true;

	sf::Mouse::setPosition(fixedMousePos, window);

	sf::Texture wallTexture;
	if (!wallTexture.loadFromFile("assets/redbrick.png")) {
		std::cout << "ERROR: Can't load wall texture." << std::endl;
		return -1;
	}

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

		std::string title = "Yay Window! FPS: ";
		title += std::to_string(1 / dt);
		window.setTitle(title);

		// setting player's direction according to mouse
		if (isFocused) {
			player.setDirection(window, fixedMousePos, dt);
			resetMousePos(window, fixedMousePos);
		}

		// getting input
		sf::Vector2f wasd;

		if (isFocused)
			wasd = wasdInput();

		player.calculateVelocity(wasd, dt);
		player.checkCollision(maze);
		player.move();

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
		for (int x = 0; x < window.getSize().x; x++) {
			angle = (player.direction() - player.fov() / 2.0f) + ((float)x / (float)window.getSize().x) * player.fov();

			// casting ray and fixing the fisheye problem
			Ray ray = raycast(player.pos(), angle, maze);
			ray.distance *= cosf(player.direction() - angle);

			if (!ray.isHit)
				continue;

			float wallHeight = ((float)window.getSize().y) / (2 * ray.distance);

			// calculating floor and ceiling y values
			float ceiling = (window.getSize().y / 2.0f) - (wallHeight / 2.0f);
			float floor = window.getSize().y - ceiling;

			// calculating shading
			sf::Color color = sf::Color::White;
			float brightness = 1.0f - (ray.distance / 8);

			// darkening the vertical walls
			if (ray.verticalHit)
				brightness *= 0.7;

			if (brightness < 0.0f)
				brightness = 0.0f;

			// apply brightness
			color.r *= brightness;
			color.g *= brightness;
			color.b *= brightness;


			float textureX = wallTexture.getSize().x * ray.hitCoord;
			sf::VertexArray wall(sf::Lines, 2);

			// setting wall position and height
			wall[0].position = { (float)x, ceiling };
			wall[1].position = { (float)x, floor };

			// shading the wall
			wall[0].color = color;
			wall[1].color = color;
			
			// texturing the wall according to the hit coordinate
			wall[0].texCoords = { textureX, 0 };
			wall[1].texCoords = { textureX, (float)wallTexture.getSize().y };

			window.draw(wall, &wallTexture);
		}

		window.display();
	}
}
