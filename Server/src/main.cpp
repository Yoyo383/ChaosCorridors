#include <iostream>
#include <unordered_map>
#include <thread>
#include <chrono>
#include "sockets.hpp"
#include "protocol.hpp"
#include "globals.hpp"
#include "maze.hpp"

constexpr unsigned short PORT = 12345;
constexpr unsigned short UDP_PORT = 54321;

constexpr int NUMBER_OF_PLAYERS = 2;

static int count = 0;

constexpr int NUMBER_OF_TICKS = 60;

struct Bullet {
	protocol::Vector2 position;
	protocol::Vector2 direction;
};

std::vector<sockets::Socket> tcpSockets;
std::vector<sockets::Address> addresses;

std::vector<std::string> names;
std::unordered_map<char, protocol::Vector2> players;
std::vector<Bullet> bullets;

globals::MazeArr maze;

template<typename T> void deleteElement(std::vector<T>& vector, T element) {
	vector.erase(std::remove(vector.begin(), vector.end(), element), vector.end());
}

template<typename T> void broadcast(T data) {
	for (auto& socket : tcpSockets)
		socket.send(data);
}
static void broadcast(std::string message) {
	for (auto& socket : tcpSockets)
		socket.send(message);
}
static void broadcast(std::vector<char> message) {
	for (auto& socket : tcpSockets)
		socket.send(message);
}

template<typename T> void broadcastUDP(sockets::Socket socket, T data) {
	for (auto& address : addresses)
		socket.sendTo<T>(data, address);
}
static void broadcastUDP(sockets::Socket socket, std::string message) {
	for (auto& address : addresses)
		socket.sendTo(message, address);
}
static void broadcastUDP(sockets::Socket socket, std::vector<char> message) {
	for (auto& address : addresses)
		socket.sendTo(message, address);
}

static void handleClient(sockets::Socket socket, sockets::Address address) {
	tcpSockets.push_back(socket);

	sockets::Address udpAddress;

	std::cout << address.ip << " " << address.port << std::endl;

	for (auto& name : names)
		socket.send("player:" + name + "\n");

	bool closed = false;

	while (!closed) {
		auto [key, value] = protocol::receiveKeyValue(socket);

		if (key == "player") {
			names.push_back(value);
			broadcast(protocol::keyValueMessage("player", value));
			socket.send(protocol::keyValueMessage("index", std::to_string(count)));
			players[count] = { 1.5f, 1.5f };
		}

		if (key == "udp") {
			udpAddress = { address.ip, (unsigned short)std::stoul(value.c_str()) };
			addresses.push_back(udpAddress);
		}

		if (key == "close") {
			deleteElement(tcpSockets, socket);
			players.erase(std::stoi(value));
			deleteElement(addresses, udpAddress);
			socket.close();
			closed = true;
			count--;
		}
	}
}

static void updateBullets() {
	for (auto& bullet : bullets) {
		bullet.position.x += bullet.direction.x * 6.0f * (1.0f / NUMBER_OF_TICKS);
		bullet.position.y += bullet.direction.y * 6.0f * (1.0f / NUMBER_OF_TICKS);
	}

	bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
		[](const Bullet& bullet) {
			if (
				bullet.position.x < 0 || bullet.position.x >= globals::WORLD_WIDTH ||
				bullet.position.y < 0 || bullet.position.y >= globals::WORLD_HEIGHT
				)
				return false;
			return maze[(int)bullet.position.y][(int)bullet.position.x] == globals::CELL_WALL;
		}
	), bullets.end());
}

static std::vector<char> mazeToVector(const globals::MazeArr& maze) {
	std::vector<char> vector;
	for (int i = 0; i < globals::WORLD_HEIGHT; i++) {
		for (int j = 0; j < globals::WORLD_WIDTH; j++) {
			vector.push_back(maze[i][j]);
		}
	}
	return vector;
}

void main() {
	srand(time(NULL));
	sockets::initialize();

	long elapsedTime = 0;
	auto lastTime = std::chrono::steady_clock::now();

	sockets::Socket serverSocket(sockets::Protocol::TCP);
	sockets::Socket udpSocket(sockets::Protocol::UDP);
	udpSocket.setBlocking(false);

	try {
		udpSocket.bind({ "0.0.0.0", UDP_PORT });
		serverSocket.bind({ "0.0.0.0", PORT });
		serverSocket.listen(4);

		while (count < NUMBER_OF_PLAYERS) {
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

		// send initial starting positions
		for (auto& address : addresses) {
			for (auto& [index, position] : players) {
				protocol::sendPositionInfo(udpSocket, address, { 0, index, position, 0 });
			}
		}

		while (count > 0) {
			auto now = std::chrono::steady_clock::now();
			elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count();

			if (elapsedTime < (1000 / NUMBER_OF_TICKS))
				continue;

			lastTime = now;
			elapsedTime = 0;


			int receivedType = -1;
			do {
				auto packet = protocol::receivePositionInfo(udpSocket);
				receivedType = packet.type;

				if (packet.type == 0) {
					players[packet.index] = packet.position;
					// setting the type to update player position
					packet.type = 3;

					for (auto& address : addresses) {
						protocol::sendPositionInfo(udpSocket, address, packet);
					}
				}
				else if (packet.type == 1) {
					bullets.push_back({ packet.position, { cosf(packet.direction), sinf(packet.direction) } });
				}
			} while (receivedType != -1);

			updateBullets();

			for (auto& address : addresses) {
				// clear the bullets
				protocol::sendPositionInfo(udpSocket, address, { 2, 0, { 0, 0 }, 0 });

				// send new bullet information
				for (int i = 0; i < bullets.size(); i++)
					protocol::sendPositionInfo(udpSocket, address, { 1, (char)i, bullets[i].position, 0 }); // direction is 0 because client ignores it
			}

		}

		std::cout << "bye bye" << std::endl;
	}
	catch (std::exception& err) {
		std::cout << "Server error: " << err.what() << std::endl;
	}

	serverSocket.close();

	sockets::shutdown();
}
