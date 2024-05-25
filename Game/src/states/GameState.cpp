#include "GameState.hpp"
#include "maze.hpp"
#include "util.hpp"
#include "globals.hpp"
#include "protocol.hpp"
#include "EndState.hpp"
#include <iostream>

struct Sprite
{
	std::string texture;
	sf::Vector2f position;
};

GameState::GameState(Members& members, bool isFocused, std::string ip)
	: members(members), maze(), isFocused(isFocused)
{
	members.udpSocket.setBlocking(false);
	members.tcpSocket.setBlocking(true);

	maze = members.tcpSocket.recv<globals::MazeArr>();

	members.tcpSocket.setBlocking(false);

	serverAddressUDP = { ip, globals::UDP_PORT };

	heartSprite.setTexture(members.textures["heart"]);

	timer = 0;

	score = 0;

	timerText.setFont(members.font);
	timerText.setPosition(0, heartSprite.getGlobalBounds().height);
	timerText.setCharacterSize(heartSprite.getGlobalBounds().height);

	scoreText.setFont(members.font);
	scoreText.setCharacterSize(heartSprite.getGlobalBounds().height);
	scoreText.setPosition(0, 2 * heartSprite.getGlobalBounds().height);

	fixedMousePos = { (int)members.window.getSize().x / 2, (int)members.window.getSize().y / 2 };
	paused = false;
	dt = 0;
	elapsedTime = 0;
	zBuffer = new float[members.window.getSize().x + 1];

	crosshair.setTexture(members.textures["crosshair"]);
	//float scale = members.window.getSize().x / 30.0f / members.textures["crosshair"].getSize().x;
	crosshair.setOrigin(crosshair.getLocalBounds().getSize() / 2);
	crosshair.setPosition((float)fixedMousePos.x, (float)fixedMousePos.y);
}

GameState::~GameState()
{
	members.window.setMouseCursorVisible(true);
	delete[] zBuffer;
}

void GameState::resetMousePos()
{
	sf::Mouse::setPosition(fixedMousePos, members.window);
}

sf::Vector2f GameState::wasdInput()
{
	return {
		(float)(sf::Keyboard::isKeyPressed(sf::Keyboard::W) - sf::Keyboard::isKeyPressed(sf::Keyboard::S)),
		(float)(sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	};
}

void GameState::setPlayerDirection()
{
	int currentMousePos = sf::Mouse::getPosition(members.window).x;
	float deltaMousePos = (currentMousePos - fixedMousePos.x) * Player::SENSITIVITY;
	player.direction += deltaMousePos;
	/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		player.direction -= 2 * dt;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		player.direction += 2 * dt;*/

	player.direction = fmodf(player.direction, 2 * M_PI);
}

void GameState::update()
{
	dt = deltaClock.restart().asSeconds();
	elapsedTime += dt;

	sf::Event event;

	members.window.setTitle(std::to_string(1 / dt));

	while (members.window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			members.manager.quit();
			members.tcpSocket.send(protocol::keyValueMessage("close", std::to_string(members.playerIndex)));
			members.tcpSocket.close();
			return;
		}
		else if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
				paused = !paused;
		}
		else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && !paused)
		{
			sf::Vector2f bulletPosition = player.pos + 0.3f * sf::Vector2f{ cosf(player.direction), sinf(player.direction) };

			protocol::Packet packet;
			packet.type = protocol::PacketType::UPDATE_BULLET;
			packet.index = members.playerIndex;
			packet.position = bulletPosition;
			packet.direction = player.direction;

			protocol::sendPacket(members.udpSocket, serverAddressUDP, packet);
		}
		else if (event.type == sf::Event::LostFocus)
			isFocused = false;
		else if (event.type == sf::Event::GainedFocus)
			isFocused = true;
	}

	std::string receivedKey = "";
	do
	{
		auto [key, value] = protocol::receiveKeyValue(members.tcpSocket);
		receivedKey = key;

		if (key == "hit")
			player.lives--;

		else if (key == "timer")
			timer = std::stoi(value);

		else if (key == "score")
			score += std::stoi(value);

		else if (key == "end")
		{
			members.window.setMouseCursorVisible(true);

			std::unique_ptr<EndState> endState = std::make_unique<EndState>(members, value);
			members.manager.setState(std::move(endState));

			members.tcpSocket.send(protocol::keyValueMessage("close", std::to_string(members.playerIndex)));
			members.tcpSocket.close();
			return;
		}

	}
	while (receivedKey != "");


	protocol::PacketType receivedType = protocol::PacketType::NO_PACKET;
	do
	{
		protocol::Packet packet = protocol::receivePacket(members.udpSocket);
		receivedType = packet.type;
		if (packet.type == protocol::PacketType::INIT_PLAYER)
		{
			if (packet.index == members.playerIndex)
			{
				player.pos = packet.position;
				player.lives = 3;
			}
			else
				players[packet.index] = packet.position;
		}

		else if (packet.type == protocol::PacketType::UPDATE_BULLET)
			bullets[packet.index] = packet.position;

		else if (packet.type == protocol::PacketType::CLEAR_BULLETS)
			bullets.clear();

		else if (packet.type == protocol::PacketType::UPDATE_PLAYER && packet.index != members.playerIndex)
			targetPlayerPositions[packet.index] = packet.position;

	}
	while (receivedType != protocol::PacketType::NO_PACKET);


	for (auto& [index, targetPosition] : targetPlayerPositions)
	{
		sf::Vector2f actualPosition = players[index];
		sf::Vector2f direction = targetPosition - actualPosition;
		if (vecMagnitude(direction) <= 0.01f)
			continue;
		players[index] += vecNormalize(direction) * dt * Player::SPEED;
	}

	if (elapsedTime >= 1 / 30.0f)
	{
		protocol::Packet packet;
		packet.type = protocol::PacketType::UPDATE_PLAYER;
		packet.index = members.playerIndex;
		packet.position = player.pos;

		protocol::sendPacket(members.udpSocket, serverAddressUDP, packet);
		elapsedTime = 0;
	}

	if (paused)
	{
		members.window.setMouseCursorVisible(true);
		return;
	}

	// setting player's direction according to mouse
	if (isFocused)
	{
		members.window.setMouseCursorVisible(false);
		setPlayerDirection();
		resetMousePos();
	}
	else
		members.window.setMouseCursorVisible(true);

	// getting input
	sf::Vector2f wasd;

	if (isFocused)
		wasd = wasdInput();

	player.calculateVelocity(wasd, dt);
	player.checkCollision(maze);
	player.move();
}

Ray GameState::raycast(float angle)
{
	// the result
	sf::Vector2f hit(-1, -1);

	// ray direction
	sf::Vector2f rayDir = { cosf(angle), sinf(angle) };

	// the unit step size
	sf::Vector2f rayUnitStepSize = {
		sqrt(1 + (rayDir.y / rayDir.x) * (rayDir.y / rayDir.x)),
		sqrt(1 + (rayDir.x / rayDir.y) * (rayDir.x / rayDir.y))
	};

	sf::Vector2i currentCell = { (int)player.pos.x, (int)player.pos.y };
	sf::Vector2f rayLength1D;
	sf::Vector2i step;

	// set step and initial ray length
	if (rayDir.x < 0)
	{
		step.x = -1;
		rayLength1D.x = (player.pos.x - float(currentCell.x)) * rayUnitStepSize.x;
	}
	else
	{
		step.x = 1;
		rayLength1D.x = (float(currentCell.x + 1) - player.pos.x) * rayUnitStepSize.x;
	}

	if (rayDir.y < 0)
	{
		step.y = -1;
		rayLength1D.y = (player.pos.y - float(currentCell.y)) * rayUnitStepSize.y;
	}
	else
	{
		step.y = 1;
		rayLength1D.y = (float(currentCell.y + 1) - player.pos.y) * rayUnitStepSize.y;
	}

	bool foundCell = false;
	bool verticalHit = false;
	float maxDistance = globals::WORLD_WIDTH;
	float distance = 0;
	float hitCoord = 0;

	// walk on the ray until collision (or distance is bigger than maxDistance)
	while (!foundCell && distance < maxDistance)
	{
		if (rayLength1D.x < rayLength1D.y)
		{
			currentCell.x += step.x;
			distance = rayLength1D.x;
			rayLength1D.x += rayUnitStepSize.x;
			verticalHit = true;
		}
		else
		{
			currentCell.y += step.y;
			distance = rayLength1D.y;
			rayLength1D.y += rayUnitStepSize.y;
			verticalHit = false;
		}

		if (currentCell.x >= 0 && currentCell.x < globals::WORLD_WIDTH && currentCell.y >= 0 && currentCell.y < globals::WORLD_HEIGHT)
		{
			if (maze[currentCell.y][currentCell.x] == globals::CELL_WALL)
			{
				foundCell = true;
			}
		}
	}

	sf::Vector2f hitPos = player.pos + rayDir * distance;

	if (verticalHit)
		hitCoord = hitPos.y;
	else
		hitCoord = hitPos.x;

	return { foundCell, verticalHit, distance, hitCoord - int(hitCoord) };
}

void GameState::draw()
{
	members.window.clear(sf::Color::Black);

	drawFloorAndCeiling();
	drawWalls();

	std::vector<Sprite> sprites;

	for (auto& [index, position] : players)
	{
		sprites.push_back({ "character", position });
	}

	for (auto& [index, position] : bullets)
	{
		sprites.push_back({ "bullet", position });
	}

	std::sort(sprites.begin(), sprites.end(),
		[this](Sprite pos1, Sprite pos2)
		{
			return vecMagnitude(pos1.position - player.pos) > vecMagnitude(pos2.position - player.pos);
		}
	);

	for (auto& sprite : sprites)
	{
		drawSprite(sprite.position, sprite.texture);
	}

	for (int i = 0; i < player.lives; i++)
	{
		heartSprite.setPosition({ i * heartSprite.getGlobalBounds().width, 0 });
		members.window.draw(heartSprite);
	}

	std::string secondsString = std::to_string(timer % 60);
	std::string leadingZeros = std::string(2 - std::min(2, (int)secondsString.length()), '0');
	// pad seconds with zeros
	std::string timerString = std::to_string(timer / 60) + ":" + leadingZeros + secondsString;
	timerText.setString(timerString);

	sf::Color timerColor;
	timerColor.g = (int)((255.0f / globals::GAME_TIME) * timer);
	timerColor.r = 255 - timerColor.g;
	timerText.setFillColor(timerColor);

	members.window.draw(timerText);

	scoreText.setString(std::to_string(score));
	members.window.draw(scoreText);

	members.window.draw(crosshair);

	members.window.display();
}

void GameState::drawFloorAndCeiling()
{
	// scale factor for d (and other stuff)
	float tan = tanf(Player::FOV / 2);

	// for doing floor/ceiling things
	float cos = cosf(player.direction), sin = sinf(player.direction);

	sf::VertexArray floorLines(sf::Lines, members.window.getSize().y + 2);
	sf::VertexArray ceilingLines(sf::Lines, members.window.getSize().y + 2);

	for (int y = 0; y <= members.window.getSize().y / 2; y++)
	{
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

		// floor

		// texturing the floor according to the start and end sample positions and the player position
		floorLines[2 * y].texCoords = endPos + player.pos * members.textures["floor"].getSize().x;
		floorLines[2 * y + 1].texCoords = startPos + player.pos * members.textures["floor"].getSize().x;
		// shading
		floorLines[2 * y].color = color;
		floorLines[2 * y + 1].color = color;
		// setting floor position
		floorLines[2 * y].position = { 0, (float)y + members.window.getSize().y / 2 };
		floorLines[2 * y + 1].position = { (float)members.window.getSize().x, (float)y + members.window.getSize().y / 2 };


		// ceiling

		// texturing the ceiling according to the start and end sample positions and the player position
		ceilingLines[2 * y].texCoords = endPos + player.pos * members.textures["ceiling"].getSize().x;
		ceilingLines[2 * y + 1].texCoords = startPos + player.pos * members.textures["ceiling"].getSize().x;
		// shading
		ceilingLines[2 * y].color = color;
		ceilingLines[2 * y + 1].color = color;
		// setting ceiling position
		ceilingLines[2 * y].position = { 0, members.window.getSize().y / 2 - (float)y };
		ceilingLines[2 * y + 1].position = { (float)members.window.getSize().x, members.window.getSize().y / 2 - (float)y };
	}

	members.window.draw(floorLines, &members.textures["floor"]);
	members.window.draw(ceilingLines, &members.textures["ceiling"]);
}

void GameState::drawWalls()
{
	// variables for angle increment
	// math taken from here:
	// https://stackoverflow.com/questions/24173966/raycasting-engine-rendering-creating-slight-distortion-increasing-towards-edges
	float screenHalfLen = tanf(Player::FOV / 2);
	float segLen = 2 * screenHalfLen / members.window.getSize().x;

	sf::VertexArray wallLines(sf::Lines, 2 * (members.window.getSize().x + 1));

	float angle;
	for (int x = 0; x <= members.window.getSize().x; x++)
	{
		// angle calculation such that the walls aren't distorted
		angle = player.direction + atanf(segLen * x - screenHalfLen);

		// casting ray and fixing the fisheye problem
		Ray ray = raycast(angle);
		ray.distance *= cosf(player.direction - angle);

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
		int textureSizeX = members.textures["wall"].getSize().x;
		float textureX = (int)(textureSizeX * ray.hitCoord * xMultiplier) % textureSizeX;

		// setting wall position and height
		wallLines[2 * x].position = { (float)x, ceiling };
		wallLines[2 * x + 1].position = { (float)x, floor };

		// shading the wall
		wallLines[2 * x].color = color;
		wallLines[2 * x + 1].color = color;

		// texturing the wall according to the hit coordinate
		wallLines[2 * x].texCoords = { textureX, 0 };
		wallLines[2 * x + 1].texCoords = { textureX, (float)members.textures["wall"].getSize().y };
	}

	members.window.draw(wallLines, &members.textures["wall"]);
}

void GameState::drawSprite(sf::Vector2f position, std::string texture)
{
	float angleFromPlayer = vecAngle(position - player.pos);
	float relativeAngle = player.direction - angleFromPlayer;

	if (relativeAngle > M_PI)
		relativeAngle -= 2 * M_PI;
	else if (relativeAngle < -M_PI)
		relativeAngle += 2 * M_PI;

	float distance = vecMagnitude(position - player.pos);
	distance *= cosf(relativeAngle);

	// check angle to prevent weird stretching
	if (distance >= 0.2f && abs(relativeAngle) < degToRad(45))
	{
		float height = (float)members.window.getSize().y / distance;

		// calculating floor and ceiling y values
		float ceiling = (members.window.getSize().y - height) / 2.0f;
		float floor = members.window.getSize().y - ceiling;

		// getting width of texture
		float aspectRatio = (float)members.textures[texture].getSize().x / members.textures[texture].getSize().y;
		float width = height * aspectRatio;

		// calculating middle of texture
		float middle = members.window.getSize().x - (relativeAngle / Player::FOV + 0.5f) * members.window.getSize().x;

		sf::VertexArray sprite(sf::Lines, 2 * (width + 1));

		for (int x = 0; x <= width; x++)
		{
			float posX = middle + (float)x - (width / 2.0f);

			// if outside window or behind walls then don't draw
			if (posX < 0 || posX > members.window.getSize().x || zBuffer[(int)posX] < distance)
				continue;

			sf::Color color = sf::Color::White;
			float brightness = 1.0f - (distance / 16);

			if (brightness < 0)
				brightness = 0;

			// apply brightness
			color.r *= brightness;
			color.g *= brightness;
			color.b *= brightness;

			// setting position and height
			sprite[2 * x].position = { posX, ceiling };
			sprite[2 * x + 1].position = { posX, floor };

			// shading
			sprite[2 * x].color = color;
			sprite[2 * x + 1].color = color;

			// texturing
			float textureX = members.textures[texture].getSize().x * (float)x / width;
			sprite[2 * x].texCoords = { textureX, 0 };
			sprite[2 * x + 1].texCoords = { textureX, (float)members.textures[texture].getSize().y };
		}

		members.window.draw(sprite, &members.textures[texture]);
	}
}
