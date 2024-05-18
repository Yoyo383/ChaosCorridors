#include "GameState.hpp"
#include "maze.hpp"
#include "../util.hpp"
#include "globals.hpp"
#include "protocol.hpp"
#include <iostream>

GameState::GameState(Members& members)
	: members(members), maze() {

	members.tcpSocket.setBlocking(true);
	members.udpSocket.setBlocking(false);

	std::vector<char> rawMaze = members.tcpSocket.recv(globals::WORLD_WIDTH * globals::WORLD_HEIGHT);
	for (int i = 0; i < rawMaze.size(); i++) {
		maze[i / globals::WORLD_WIDTH][i % globals::WORLD_WIDTH] = rawMaze[i];
	}

	serverAddress = { "127.0.0.1", 54321 };

	fixedMousePos = { (int)members.window.getSize().x / 2, (int)members.window.getSize().y / 2 };
	isFocused = true;
	paused = false;
	dt = 0;
	elapsedTime = 0;
	zBuffer = new float[members.window.getSize().x + 1];

}

GameState::~GameState() {
	members.window.setMouseCursorVisible(true);
	delete[] zBuffer;
}

void GameState::resetMousePos() {
	sf::Mouse::setPosition(fixedMousePos, members.window);
}

sf::Vector2f GameState::wasdInput() {
	return {
		(float)(sf::Keyboard::isKeyPressed(sf::Keyboard::W) - sf::Keyboard::isKeyPressed(sf::Keyboard::S)),
		(float)(sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	};
}

void GameState::update() {
	dt = deltaClock.restart().asSeconds();
	elapsedTime += dt;
	sf::Event event;

	members.window.setTitle(std::to_string(1 / dt));

	while (members.window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			members.manager.quit();
			members.tcpSocket.send(protocol::keyValueMessage("close", std::to_string(members.playerIndex)));
			members.tcpSocket.close();
			return;
		}
		else if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape)
				paused = !paused;
		}
		else if (event.type == sf::Event::KeyReleased) {
			if (event.key.code == sf::Keyboard::Space) {
				sf::Vector2f bulletPosition = player.getPos() + 0.3f * sf::Vector2f{ cosf(player.getDirection()), sinf(player.getDirection()) };
				protocol::Vector2 position = { bulletPosition.x, bulletPosition.y };
				protocol::sendPositionInfo(members.udpSocket, serverAddress, { 1, 0, position, player.getDirection() });
			}
		}
		else if (event.type == sf::Event::LostFocus)
			isFocused = false;
		else if (event.type == sf::Event::GainedFocus)
			isFocused = true;
	}

	int receivedType = -1;
	do {
		auto [type, index, position, direction] = protocol::receivePositionInfo(members.udpSocket);
		receivedType = type;
		if (type == 0 && index != members.playerIndex)
			players[index] = { position.x, position.y };
		else if (type == 1)
			bullets[index] = { position.x, position.y };
		else if (type == 2)
			bullets.clear();

	} while (receivedType != -1);


	if (paused) {
		members.window.setMouseCursorVisible(true);
		return;
	}

	// setting player's direction according to mouse
	if (isFocused) {
		//window.setMouseCursorVisible(false);
		player.setDirection(members.window, fixedMousePos, dt);
		//resetMousePos();
	}
	else
		members.window.setMouseCursorVisible(true);

	// getting input
	sf::Vector2f wasd;

	if (isFocused)
		wasd = wasdInput();

	player.calculateVelocity(wasd, dt);
	player.checkCollision(maze);
	bool moved = player.move();

	if (moved)
		protocol::sendPositionInfo(members.udpSocket, serverAddress, { 0, members.playerIndex, { player.getPos().x, player.getPos().y } });
}

void GameState::draw() {
	members.window.clear(sf::Color::Black);

	drawFloorAndCeiling();
	drawWalls();

	std::vector<sf::Vector2f> playerPositions;

	for (auto& [index, position] : players) {
		playerPositions.push_back(position);
	}

	std::sort(playerPositions.begin(), playerPositions.end(), 
		[this](sf::Vector2f pos1, sf::Vector2f pos2) {
			return vecMagnitude(pos1 - player.getPos()) > vecMagnitude(pos2 - player.getPos());
		}
	);
	
	for (auto& position : playerPositions) {
		drawSprite(position, "character");
	}

	for (auto& [index, bullet] : bullets) {
		drawSprite(bullet, "bullet");
	}

	members.window.display();
}

void GameState::drawFloorAndCeiling() {
	// scale factor for d (and other stuff)
	float tan = tanf(player.getFOV() / 2);

	// for doing floor/ceiling things
	float cos = cosf(player.getDirection()), sin = sinf(player.getDirection());

	for (int y = 0; y <= members.window.getSize().y / 2; y++) {
		// the distance of the current row
		// floor and ceiling texture have the same size so it doesn't matter which size is in the formula
		int d = members.textures["floor"].getSize().x * members.window.getSize().y / 2 / (y + 1);

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
		float brightness = 1.0f - (float)d / members.window.getSize().y;
		if (brightness < 0)
			brightness = 0;
		color.r *= brightness;
		color.g *= brightness;
		color.b *= brightness;


		sf::VertexArray scanLine(sf::Lines, 2);

		// texturing the floor/ceiling according to the start and end sample positions and the player position
		scanLine[0].texCoords = endPos + player.getPos() * members.textures["floor"].getSize().x;
		scanLine[1].texCoords = startPos + player.getPos() * members.textures["floor"].getSize().x;
		// shading
		scanLine[0].color = color;
		scanLine[1].color = color;


		// setting floor position
		scanLine[0].position = { 0, (float)y + members.window.getSize().y / 2 };
		scanLine[1].position = { (float)members.window.getSize().x, (float)y + members.window.getSize().y / 2 };

		members.window.draw(scanLine, &members.textures["floor"]);


		// setting ceiling position
		scanLine[0].position = { 0, members.window.getSize().y / 2 - (float)y };
		scanLine[1].position = { (float)members.window.getSize().x, members.window.getSize().y / 2 - (float)y };

		members.window.draw(scanLine, &members.textures["ceiling"]);
	}
}

void GameState::drawWalls() {
	// variables for angle increment
	// math taken from here:
	// https://stackoverflow.com/questions/24173966/raycasting-engine-rendering-creating-slight-distortion-increasing-towards-edges
	float screenHalfLen = tanf(player.getFOV() / 2);
	float segLen = 2 * screenHalfLen / members.window.getSize().x;

	float angle;
	for (int x = 0; x <= members.window.getSize().x; x++) {
		// angle calculation such that the walls aren't distorted
		angle = player.getDirection() + atanf(segLen * x - screenHalfLen);

		// casting ray and fixing the fisheye problem
		Ray ray = raycast(player.getPos(), angle, maze);
		ray.distance *= cosf(player.getDirection() - angle);

		zBuffer[x] = ray.distance;

		if (!ray.isHit)
			continue;

		float wallHeight = (float)members.window.getSize().y / ray.distance;

		// calculating floor and ceiling y values
		float ceiling = (members.window.getSize().y - wallHeight) / 2.0f;
		float floor = members.window.getSize().y - ceiling;

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
		float xMultiplier = (float)members.window.getSize().x / members.window.getSize().y;
		// the x coord to sample from in the texture
		float textureX = (int)(members.textures["wall"].getSize().x * ray.hitCoord * xMultiplier) % members.textures["wall"].getSize().x;


		sf::VertexArray wall(sf::Lines, 2);

		// setting wall position and height
		wall[0].position = { (float)x, ceiling };
		wall[1].position = { (float)x, floor };

		// shading the wall
		wall[0].color = color;
		wall[1].color = color;

		// texturing the wall according to the hit coordinate
		wall[0].texCoords = { textureX, 0 };
		wall[1].texCoords = { textureX, (float)members.textures["wall"].getSize().y };

		members.window.draw(wall, &members.textures["wall"]);
	}
}

void GameState::drawSprite(const sf::Vector2f& characterPos, std::string texture) {
	float angleFromPlayer = vecAngle(characterPos - player.getPos());
	float relativeAngle = player.getDirection() - angleFromPlayer;

	if (relativeAngle > M_PI)
		relativeAngle -= 2 * M_PI;
	else if (relativeAngle < -M_PI)
		relativeAngle += 2 * M_PI;

	float distance = vecMagnitude(characterPos - player.getPos());
	distance *= cosf(relativeAngle);

	if (distance >= 0.2f) {
		float height = (float)members.window.getSize().y / distance;

		// calculating floor and ceiling y values
		float ceiling = (members.window.getSize().y - height) / 2.0f;
		float floor = members.window.getSize().y - ceiling;

		// getting width of texture
		float aspectRatio = (float)members.textures[texture].getSize().x / members.textures[texture].getSize().y;
		float width = height * aspectRatio;

		// calculating middle of texture
		float middle = members.window.getSize().x - (relativeAngle / player.getFOV() + 0.5f) * members.window.getSize().x;

		for (int x = 0; x <= width; x++) {
			float posX = middle + (float)x - (width / 2.0f);

			// if outside window or behind walls then don't draw
			if (posX < 0 || posX > members.window.getSize().x || zBuffer[(int)posX] < distance)
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
			float textureX = members.textures[texture].getSize().x * (float)x / width;
			character[0].texCoords = { textureX, 0 };
			character[1].texCoords = { textureX, (float)members.textures[texture].getSize().y };

			members.window.draw(character, &members.textures[texture]);
		}
	}
}
