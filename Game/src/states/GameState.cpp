#include "GameState.hpp"
#include "maze.hpp"
#include "../util.hpp"
#include "globals.hpp"
#include <iostream>

GameState::GameState(StateManager& manager, sf::RenderWindow& window, TextureManager& textures, sockets::Socket socket)
	: State{ manager, window, textures }, socket(socket), maze() {

	socket.setBlocking(true);
	std::vector<char> rawMaze = socket.recv(globals::WORLD_WIDTH * globals::WORLD_HEIGHT);
	socket.setBlocking(false);

	for (int i = 0; i < rawMaze.size(); i++) {
		maze[i / globals::WORLD_WIDTH][i % globals::WORLD_WIDTH] = rawMaze[i];
	}

	fixedMousePos = { (int)window.getSize().x / 2, (int)window.getSize().y / 2 };
	isFocused = true;
	paused = false;
	dt = 0;
	zBuffer = new float[window.getSize().x + 1];

}

GameState::~GameState() {
	window.setMouseCursorVisible(true);
	delete[] zBuffer;
}

void GameState::resetMousePos() {
	sf::Mouse::setPosition(fixedMousePos, window);
}

sf::Vector2f GameState::wasdInput() {
	return {
		(float)(sf::Keyboard::isKeyPressed(sf::Keyboard::W) - sf::Keyboard::isKeyPressed(sf::Keyboard::S)),
		(float)(sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	};
}

void GameState::update() {
	dt = deltaClock.restart().asSeconds();
	sf::Event event;

	window.setTitle(std::to_string(1 / dt));

	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			manager.quit();
			socket.close();
			paused = true;
		}
		else if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape)
				paused = !paused;
		}
		else if (event.type == sf::Event::LostFocus)
			isFocused = false;
		else if (event.type == sf::Event::GainedFocus)
			isFocused = true;
	}

	if (paused) {
		window.setMouseCursorVisible(true);
		return;
	}

	// setting player's direction according to mouse
	if (isFocused) {
		window.setMouseCursorVisible(false);
		player.setDirection(window, fixedMousePos);
		resetMousePos();
	}
	else
		window.setMouseCursorVisible(true);

	// getting input
	sf::Vector2f wasd;

	if (isFocused)
		wasd = wasdInput();

	player.calculateVelocity(wasd, dt);
	player.checkCollision(maze);
	player.move();
}

void GameState::draw() {
	window.clear(sf::Color::Black);

	drawFloorAndCeiling();
	drawWalls();
	drawCharacter({ 1.5f, 1.5f });

	window.display();
}

void GameState::drawFloorAndCeiling() {
	// scale factor for d (and other stuff)
	float tan = tanf(player.getFOV() / 2);

	// for doing floor/ceiling things
	float cos = cosf(player.getDirection()), sin = sinf(player.getDirection());

	for (int y = 0; y <= window.getSize().y / 2; y++) {
		// the distance of the current row
		// floor and ceiling texture have the same size so it doesn't matter which size is in the formula
		int d = textures["floor"].getSize().x * window.getSize().y / 2 / (y + 1);

		sf::Vector2f startPos = {
			d * cos - d * tan * sin,
			d * sin + d * tan * cos
		};
		sf::Vector2f endPos = {
			d * cos + d * tan * sin,
			d * sin - d * tan * cos
		};

		// calculating shading based on distance
		sf::Color color = sf::Color::White;
		float brightness = 1.0f - (float)d / window.getSize().y;
		if (brightness < 0)
			brightness = 0;
		color.r *= brightness;
		color.g *= brightness;
		color.b *= brightness;


		sf::VertexArray scanLine(sf::Lines, 2);

		// texturing the floor/ceiling according to the start and end sample positions and the player position
		scanLine[0].texCoords = endPos + player.getPos() * textures["floor"].getSize().x;
		scanLine[1].texCoords = startPos + player.getPos() * textures["floor"].getSize().x;
		// shading
		scanLine[0].color = color;
		scanLine[1].color = color;


		// setting floor position
		scanLine[0].position = { 0, (float)y + window.getSize().y / 2 };
		scanLine[1].position = { (float)window.getSize().x, (float)y + window.getSize().y / 2 };

		window.draw(scanLine, &textures["floor"]);


		// setting ceiling position
		scanLine[0].position = { 0, window.getSize().y / 2 - (float)y };
		scanLine[1].position = { (float)window.getSize().x, window.getSize().y / 2 - (float)y };

		window.draw(scanLine, &textures["ceiling"]);
	}
}

void GameState::drawWalls() {
	// variables for angle increment
	// math taken from here:
	// https://stackoverflow.com/questions/24173966/raycasting-engine-rendering-creating-slight-distortion-increasing-towards-edges
	float screenHalfLen = tanf(player.getFOV() / 2);
	float segLen = 2 * screenHalfLen / window.getSize().x;

	float angle;
	for (int x = 0; x <= window.getSize().x; x++) {
		// angle calculation such that the walls aren't distorted
		angle = player.getDirection() + atanf(segLen * x - screenHalfLen);

		// casting ray and fixing the fisheye problem
		Ray ray = raycast(player.getPos(), angle, maze);
		ray.distance *= cosf(player.getDirection() - angle);

		zBuffer[x] = ray.distance;

		if (!ray.isHit)
			continue;

		float wallHeight = (float)window.getSize().y / ray.distance;

		// calculating floor and ceiling y values
		float ceiling = (window.getSize().y - wallHeight) / 2.0f;
		float floor = window.getSize().y - ceiling;

		// calculating shading
		sf::Color color = sf::Color::White;
		float brightness = 1.0f - (ray.distance / 16);

		// darkening the vertical walls
		if (ray.verticalHit)
			brightness *= 0.7;

		if (brightness < 0)
			brightness = 0;

		// apply brightness
		color.r *= brightness;
		color.g *= brightness;
		color.b *= brightness;

		// making sure the texture is the right aspect ration
		float xMultiplier = (float)window.getSize().x / window.getSize().y;
		// the x coord to sample from in the texture
		float textureX = (int)(textures["wall"].getSize().x * ray.hitCoord * xMultiplier) % textures["wall"].getSize().x;


		sf::VertexArray wall(sf::Lines, 2);

		// setting wall position and height
		wall[0].position = { (float)x, ceiling };
		wall[1].position = { (float)x, floor };

		// shading the wall
		wall[0].color = color;
		wall[1].color = color;

		// texturing the wall according to the hit coordinate
		wall[0].texCoords = { textureX, 0 };
		wall[1].texCoords = { textureX, (float)textures["wall"].getSize().y };

		window.draw(wall, &textures["wall"]);
	}
}

void GameState::drawCharacter(const sf::Vector2f& characterPos) {
	float angleFromPlayer = vecAngle(characterPos - player.getPos());
	float relativeAngle = player.getDirection() - angleFromPlayer;

	if (relativeAngle > M_PI)
		relativeAngle -= 2 * M_PI;
	else if (relativeAngle < -M_PI)
		relativeAngle += 2 * M_PI;

	float distance = vecMagnitude(characterPos - player.getPos());
	distance *= cosf(relativeAngle);

	if (distance >= 0.5f) {
		float height = (float)window.getSize().y / distance;

		// calculating floor and ceiling y values
		float ceiling = (window.getSize().y - height) / 2.0f;
		float floor = window.getSize().y - ceiling;

		// getting width of texture
		float aspectRatio = (float)textures["character"].getSize().x / textures["character"].getSize().y;
		float width = height * aspectRatio;

		// calculating middle of texture
		float middle = window.getSize().x - (relativeAngle / player.getFOV() + 0.5f) * window.getSize().x;

		for (int x = 0; x <= width; x++) {
			float posX = middle + (float)x - (width / 2.0f);

			// if outside window or behind walls then don't draw
			if (posX < 0 || posX > window.getSize().x || zBuffer[(int)posX] < distance)
				continue;

			sf::VertexArray character(sf::Lines, 2);

			sf::Color color = sf::Color::White;
			float brightness = 1.0f - (distance / 16);

			if (brightness < 0)
				brightness = 0;

			// apply brightness
			color.r *= brightness;
			color.g *= brightness;
			color.b *= brightness;

			// setting position and height
			character[0].position = { posX, ceiling };
			character[1].position = { posX, floor };

			// shading
			character[0].color = color;
			character[1].color = color;

			// texturing
			float textureX = textures["character"].getSize().x * (float)x / width;
			character[0].texCoords = { textureX, 0 };
			character[1].texCoords = { textureX, (float)textures["character"].getSize().y };

			window.draw(character, &textures["character"]);
		}
	}
}
