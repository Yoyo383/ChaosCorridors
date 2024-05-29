#include <iostream>
#include <unordered_map>
#include <thread>
#include <chrono>
#include <functional>
#include <mutex>
#include "sockets.hpp"
#include "protocol.hpp"
#include "globals.hpp"
#include "maze.hpp"
#include "Player.hpp"

// for easier use
using time_point = std::chrono::steady_clock::time_point;

struct Bullet
{
	int playerIndex;
	sf::Vector2f position;
	sf::Vector2f direction;
};

struct Client
{
	sockets::Socket tcpSocket;
	sockets::Address udpAddress;
	Player player;
	std::string name;
	int score = 0;
};

const int NUMBER_OF_TICKS = 60;
const int KILL_PLAYER_SCORE = 100;
const float BULLET_SPEED = 10.0f;

int numberOfPlayers = 0;

std::atomic<int> count = 0;

std::vector<Bullet> bullets;

std::vector<std::thread> clientThreads;

std::unordered_map<int, Client> clients;
std::mutex clientsMutex;

globals::MazeArr maze;

int timer = globals::GAME_TIME;


static void forEachUDP(std::function<void(sockets::Address)> function)
{
	for (auto& [index, client] : clients)
		function(client.udpAddress);
}

template<typename T> void broadcast(T data)
{
	for (auto& [index, client] : clients)
		client.tcpSocket.send(data);
}

static sf::Vector2f randomPosition()
{
	sf::Vector2f position;
	do
	{
		position =
		{
			randInt(1, globals::WORLD_WIDTH - 2) + 0.5f,
			randInt(1, globals::WORLD_HEIGHT - 2) + 0.5f
		};
	}
	while (maze[(int)position.y][(int)position.x] != globals::CELL_EMPTY);

	return position;
}


static void handleClient(sockets::Socket socket, sockets::Address address)
{
	sockets::Address udpAddress;
	int index = 0;

	for (auto& [index, client] : clients)
		socket.send(protocol::keyValueMessage("player", client.name));

	bool closed = false;

	while (!closed)
	{
		try
		{
			auto [key, value] = protocol::receiveKeyValue(socket);

			if (key == "player") // value is the name
			{
				index = count;
				socket.send(protocol::keyValueMessage("index", std::to_string(index)));

				std::lock_guard lock(clientsMutex);
				clients[index] = Client{ socket, {"", 0}, Player(randomPosition()), value, 0};

				broadcast(protocol::keyValueMessage("player", value));
			}

			else if (key == "udp") // value is the UDP port
			{
				std::lock_guard lock(clientsMutex);
				udpAddress = { address.ip, (unsigned short)std::stoul(value.c_str()) };
				clients[index].udpAddress = udpAddress;
			}

			else if (key == "close" || key == "") // no value
			{
				std::lock_guard lock(clientsMutex);
				socket.close();
				clients.erase(index);
				std::cout << "deleting index " << index << std::endl;
				closed = true;
				count--;
				broadcast(protocol::keyValueMessage("exit", std::to_string(index)));
			}
		}
		catch (sockets::exception& err)
		{
			std::cout << err.what() << std::endl;
		}
	}
}

static void bulletPlayerCollision(int index, Client& client, Bullet& bullet, const sockets::Socket& udpSocket)
{
	client.player.lives--;
	if (client.player.lives == 0)
	{
		clients[bullet.playerIndex].tcpSocket.send(protocol::keyValueMessage("score", std::to_string(KILL_PLAYER_SCORE)));
		clients[bullet.playerIndex].score += KILL_PLAYER_SCORE;

		client.player.pos = randomPosition();
		client.player.lives = 3;

		protocol::Packet packet;
		packet.type = protocol::PacketType::INIT_PLAYER;
		packet.index = index;
		packet.position = client.player.pos;

		forEachUDP(
			[packet, udpSocket](sockets::Address address)
			{
				protocol::sendPacket(udpSocket, address, packet);
			}
		);
	}
	else // if player got hit remove a life and notify the player
		client.tcpSocket.send(protocol::keyValueMessage("hit", ""));

	// set position to delete the bullet
	bullet.position.x = -1;
}

static void updateBullets(const sockets::Socket& udpSocket)
{
	for (auto& bullet : bullets)
	{
		// move the bullets
		bullet.position.x += bullet.direction.x * BULLET_SPEED * (1.0f / NUMBER_OF_TICKS);
		bullet.position.y += bullet.direction.y * BULLET_SPEED * (1.0f / NUMBER_OF_TICKS);

		std::lock_guard lock(clientsMutex);

		for (auto& [index, client] : clients)
		{
			sf::Vector2f distance = client.player.pos - bullet.position;
			if (vecMagnitude(distance) <= 0.2f && index != bullet.playerIndex)
				bulletPlayerCollision(index, client, bullet, udpSocket);
		}
	}

	// remove bullets that are outside the map or collide with the map
	bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
		[](const Bullet& bullet)
		{
			if (
				bullet.position.x < 0 || bullet.position.x >= globals::WORLD_WIDTH ||
				bullet.position.y < 0 || bullet.position.y >= globals::WORLD_HEIGHT
				)
				return false;
			return maze[(int)bullet.position.y][(int)bullet.position.x] == globals::CELL_WALL;
		}
	), bullets.end());
}

static void initGame(const sockets::Socket& udpSocket)
{
	std::cout << "Game is starting!" << std::endl;

	// sending to clients to notify them the game began
	broadcast(protocol::keyValueMessage("start", ""));

	// send maze
	broadcast(maze);

	// send initial timer
	broadcast(protocol::keyValueMessage("timer", std::to_string(timer)));

	// send initial starting positions
	forEachUDP(
		[udpSocket](sockets::Address address)
		{
			for (auto& [index, client] : clients)
			{
				protocol::Packet packet;
				packet.type = protocol::PacketType::INIT_PLAYER;
				packet.index = index;
				packet.position = client.player.pos;

				protocol::sendPacket(udpSocket, address, packet);
			}
		}
	);
}

static bool checkDelay(int& elapsedTime, time_point& lastTimeFrame, time_point now)
{
	elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTimeFrame).count();

	if (elapsedTime < (1000 / NUMBER_OF_TICKS))
		return false;

	lastTimeFrame = now;
	elapsedTime = 0;

	return true;
}

static bool sendTimerUpdate(int& timerTime, time_point& lastTimeTimer, time_point now)
{
	timerTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTimeTimer).count();
	if (timerTime >= 1000)
	{
		lastTimeTimer = now;
		timerTime = 0;

		timer--;
		std::lock_guard lock(clientsMutex);
		broadcast(protocol::keyValueMessage("timer", std::to_string(timer)));
		if (timer == 0)
			return true;
	}
	return false;
}

static void handleEvents(const sockets::Socket& udpSocket)
{
	protocol::PacketType receivedType = protocol::PacketType::NO_PACKET;

	// receive until received NO_PACKET
	do
	{
		auto packet = protocol::receivePacket(udpSocket);
		receivedType = packet.type;

		if (packet.type == protocol::PacketType::UPDATE_PLAYER)
		{
			clients[packet.index].player.pos = packet.position;

			forEachUDP(
				[udpSocket, packet](sockets::Address address)
				{
					protocol::sendPacket(udpSocket, address, packet);
				}
			);

		}
		else if (packet.type == protocol::PacketType::UPDATE_BULLET)
			bullets.push_back({ packet.index, packet.position, { cosf(packet.direction), sinf(packet.direction) } });

	}
	while (receivedType != protocol::PacketType::NO_PACKET);
}

static void sendBullets(const sockets::Socket& udpSocket)
{
	forEachUDP(
		[udpSocket](sockets::Address address)
		{
			 // clear the bullets
			protocol::Packet packet;
			packet.type = protocol::PacketType::CLEAR_BULLETS;
			protocol::sendPacket(udpSocket, address, packet);

			packet.type = protocol::PacketType::UPDATE_BULLET;

			// send new bullet information
			for (int i = 0; i < bullets.size(); i++)
			{
				packet.index = (char)i;
				packet.position = bullets[i].position;

				protocol::sendPacket(udpSocket, address, packet);
			}
		}
	);
}

static void sendWin()
{
	std::string wonPlayers;
	int maxScore = 0;

	for (auto& [index, client] : clients)
		maxScore = max(client.score, maxScore);

	for (auto& [index, client] : clients)
	{
		if (client.score == maxScore)
		{
			if (wonPlayers == "")
				wonPlayers = client.name;
			else
				wonPlayers += " + " + client.name;
		}
	}

	wonPlayers += " won!";

	broadcast(protocol::keyValueMessage("end", wonPlayers));
}

static bool parseNumberOfPlayers(const std::string& input)
{
	try
	{
		int inputInt = std::stoi(input);
		if (inputInt <= 0)
			return false;
		numberOfPlayers = inputInt;
	}
	catch (std::invalid_argument)
	{
		return false;
	}
	return true;
}

void main()
{
	sockets::initialize();

	std::string input;

	std::cout << "Enter number of players: ";
	std::cin >> input;

	while (!parseNumberOfPlayers(input))
	{
		std::cout << "Invalid input." << std::endl;
		std::cout << "Enter number of players: ";
		std::cin >> input;
	}

	maze = globals::generateMaze();

	sockets::Socket serverSocket(sockets::Protocol::TCP);
	sockets::Socket udpSocket(sockets::Protocol::UDP);
	udpSocket.setBlocking(false);

	try
	{
		udpSocket.bind({ "0.0.0.0", globals::UDP_PORT });
		serverSocket.bind({ "0.0.0.0", globals::TCP_PORT });
		serverSocket.listen(numberOfPlayers);

		std::cout << "Waiting for connections..." << std::endl;

		while (count < numberOfPlayers)
		{
			auto [clientSocket, clientAddress] = serverSocket.accept();
			clientSocket.setTimeout(0);
			clientThreads.push_back(std::thread(handleClient, clientSocket, clientAddress));
			std::cout << "New connection at " << clientAddress.ip << ":" << clientAddress.port << std::endl;
			count++;
		}

		// closing to prevent new players to connect during the game
		serverSocket.close();

		std::this_thread::sleep_for(std::chrono::seconds(2));
		initGame(udpSocket);

		// to do the loop 60 times per second
		int elapsedTime = 0;
		auto lastTimeFrame = std::chrono::steady_clock::now();

		// to send every second an update for the timer
		int timerTime = 0;
		auto lastTimeTimer = std::chrono::steady_clock::now();

		while (count > 0)
		{
			auto now = std::chrono::steady_clock::now();

			if (!checkDelay(elapsedTime, lastTimeFrame, now))
				continue;

			bool finished = sendTimerUpdate(timerTime, lastTimeTimer, now);
			if (finished)
				sendWin();
			else
			{
				handleEvents(udpSocket);
				updateBullets(udpSocket);
				sendBullets(udpSocket);
			}
		}

		for (auto& thread : clientThreads)
			thread.join();

		std::cout << "Game ended!" << std::endl;
	}
	catch (sockets::exception& err)
	{
		std::cout << "Server error: " << err.what() << std::endl;
	}

	sockets::shutdown();
}
