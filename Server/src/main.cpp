#include <iostream>
#include <unordered_map>
#include <thread>
#include <chrono>
#include "sockets.hpp"
#include "protocol.hpp"
#include "globals.hpp"
#include "maze.hpp"
#include "Player.hpp"

constexpr unsigned short PORT = 12345;
constexpr unsigned short UDP_PORT = 54321;

constexpr int NUMBER_OF_PLAYERS = 2;

static int count = 0;

constexpr int NUMBER_OF_TICKS = 60;

struct Bullet
{
	sf::Vector2f position;
	sf::Vector2f direction;
};

std::unordered_map<char, sockets::Socket> tcpSockets;
std::vector<sockets::Address> addresses;

std::vector<std::string> names;
std::unordered_map<char, Player> players;
std::vector<Bullet> bullets;

globals::MazeArr maze;

template<typename T> void deleteElement(std::vector<T>& vector, T element)
{
	vector.erase(std::remove(vector.begin(), vector.end(), element), vector.end());
}

template<typename T> void broadcast(T data)
{
	for (auto& [index, socket] : tcpSockets)
		socket.send(data);
}

template<typename T> void broadcastUDP(sockets::Socket socket, T data)
{
	for (auto& address : addresses)
		socket.sendTo(data, address);
}

static void handleClient(sockets::Socket socket, sockets::Address address)
{
	sockets::Address udpAddress;
	std::string name;

	std::cout << address.ip << " " << address.port << std::endl;

	for (auto& name : names)
		socket.send("player:" + name + "\n");

	bool closed = false;

	while (!closed)
	{
		auto [key, value] = protocol::receiveKeyValue(socket);

		if (key == "player")
		{
			name = value;
			names.push_back(value);
			socket.send(protocol::keyValueMessage("index", std::to_string(count)));

			players[count] = Player(
				randInt(1, globals::WORLD_WIDTH - 2) + 0.5f, 
				randInt(1, globals::WORLD_HEIGHT - 2) + 0.5f
			);
			tcpSockets[count] = socket;

			broadcast(protocol::keyValueMessage("player", value));
		}

		if (key == "udp")
		{
			udpAddress = { address.ip, (unsigned short)std::stoul(value.c_str()) };
			addresses.push_back(udpAddress);
		}

		if (key == "close")
		{
			tcpSockets.erase((char)std::stoi(value));
			players.erase(std::stoi(value));
			deleteElement(addresses, udpAddress);
			deleteElement(names, name);
			socket.close();
			closed = true;
			count--;
		}
	}
}

static void updateBullets()
{
	for (auto& bullet : bullets)
	{
		bullet.position.x += bullet.direction.x * 6.0f * (1.0f / NUMBER_OF_TICKS);
		bullet.position.y += bullet.direction.y * 6.0f * (1.0f / NUMBER_OF_TICKS);

		for (auto& [index, player] : players)
		{
			sf::Vector2f distance = player.pos - bullet.position;
			if (vecMagnitude(distance) <= 0.2f)
			{
				tcpSockets[index].send(protocol::keyValueMessage("hit", ""));
				player.lives--;

				// set position to delete the bullet
				bullet.position.x = -1;

				if (player.lives == 0)
					std::cout << "oops dead" << std::endl;
			}
		}
	}

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

void main()
{
	srand(time(NULL));
	sockets::initialize();

	// how many seconds for the game
	int timer = 100;

	sockets::Socket serverSocket(sockets::Protocol::TCP);
	sockets::Socket udpSocket(sockets::Protocol::UDP);
	udpSocket.setBlocking(false);

	try
	{
		udpSocket.bind({ "0.0.0.0", UDP_PORT });
		serverSocket.bind({ "0.0.0.0", PORT });
		serverSocket.listen(4);

		while (count < NUMBER_OF_PLAYERS)
		{
			auto [clientSocket, clientAddress] = serverSocket.accept();
			clientSocket.setTimeout(0);
			std::thread thread(handleClient, clientSocket, clientAddress);
			thread.detach();
			count++;
		}

		std::this_thread::sleep_for(std::chrono::seconds(2));
		std::cout << "start!" << std::endl;
		broadcast(protocol::keyValueMessage("start", ""));

		maze = globals::generateMaze();
		broadcast(maze);

		broadcast(protocol::keyValueMessage("timer", std::to_string(timer)));

		// send initial starting positions
		for (auto& address : addresses)
		{
			for (auto& [index, player] : players)
			{
				protocol::PositionInfoPacket packet;
				packet.type = protocol::PacketType::NEW_PLAYER;
				packet.index = index;
				packet.position = player.pos;

				protocol::sendPositionInfo(udpSocket, address, packet);
			}
		}

		// to do the loop 60 times per second
		long elapsedTime = 0;
		auto lastTimeFrame = std::chrono::steady_clock::now();

		// to send every second an update for the timer
		long timerTime = 0;
		auto lastTimeTimer = std::chrono::steady_clock::now();

		while (count > 0)
		{
			auto now = std::chrono::steady_clock::now();
			elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTimeFrame).count();
			timerTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTimeTimer).count();

			if (elapsedTime < (1000 / NUMBER_OF_TICKS))
				continue;

			lastTimeFrame = now;
			elapsedTime = 0;

			if (timerTime >= 1000)
			{
				lastTimeTimer = now;
				timerTime = 0;

				timer--;
				broadcast(protocol::keyValueMessage("timer", std::to_string(timer)));
			}

			protocol::PacketType receivedType = protocol::PacketType::NO_PACKET;
			do
			{
				auto packet = protocol::receivePositionInfo(udpSocket);
				receivedType = packet.type;

				if (packet.type == protocol::PacketType::UPDATE_PLAYER)
				{
					players[packet.index].pos = packet.position;

					for (auto& address : addresses)
					{
						protocol::sendPositionInfo(udpSocket, address, packet);
					}
				}
				else if (packet.type == protocol::PacketType::UPDATE_BULLET)
				{
					bullets.push_back({ packet.position, { cosf(packet.direction), sinf(packet.direction) } });
				}
			} while (receivedType != protocol::PacketType::NO_PACKET);

			updateBullets();

			for (auto& address : addresses)
			{
				// clear the bullets
				protocol::PositionInfoPacket packet;
				packet.type = protocol::PacketType::CLEAR_BULLETS;
				protocol::sendPositionInfo(udpSocket, address, packet);

				// send new bullet information
				for (int i = 0; i < bullets.size(); i++)
				{
					protocol::PositionInfoPacket packet;
					packet.type = protocol::PacketType::UPDATE_BULLET;
					packet.index = (char)i;
					packet.position = bullets[i].position;

					protocol::sendPositionInfo(udpSocket, address, packet);
				}
			}

		}

		std::cout << "bye bye" << std::endl;
	}
	catch (std::exception& err)
	{
		std::cout << "Server error: " << err.what() << std::endl;
	}

	serverSocket.close();

	sockets::shutdown();
}
