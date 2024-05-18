#include <iostream>
#include <unordered_map>
#include <thread>
#include "sockets.hpp"
#include "protocol.hpp"
#include "globals.hpp"
#include "maze.hpp"

constexpr unsigned short PORT = 12345;
constexpr unsigned short UDP_PORT = 54321;

static int count = 0;

std::vector<sockets::Socket> tcpSockets;
std::vector<sockets::Address> addresses;

std::vector<std::string> names;
std::unordered_map<char, protocol::Position> players;

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

	sockets::Socket serverSocket(sockets::Protocol::TCP);
	sockets::Socket udpSocket(sockets::Protocol::UDP);
	udpSocket.setBlocking(false);

	try {
		udpSocket.bind({ "0.0.0.0", UDP_PORT });
		serverSocket.bind({ "0.0.0.0", PORT });
		serverSocket.listen(4);

		while (count < 2) {
			auto [clientSocket, clientAddress] = serverSocket.accept();
			clientSocket.setTimeout(0);
			std::thread thread(handleClient, clientSocket, clientAddress);
			thread.detach();
			count++;
		}

		std::this_thread::sleep_for(std::chrono::seconds(2));
		std::cout << "start!" << std::endl;
		broadcast(protocol::keyValueMessage("start", ""));
		broadcast(globals::generateMaze());

		// send initial starting positions
		for (auto& address : addresses) {
			for (auto& [index, position] : players) {
				protocol::sendPlayerPosition(udpSocket, address, { index, position });
			}
		}

		while (count > 0) {
			try {
				auto packet = protocol::receivePlayer(udpSocket);

				if (packet.playerIndex != -1) {
					players[packet.playerIndex] = packet.position;

					for (auto& address : addresses) {
						protocol::sendPlayerPosition(udpSocket, address, packet);
					}
				}

			}
			catch (std::exception& err) {
				if (err.what() != std::to_string(WSAEWOULDBLOCK))
					std::cout << "UDP recv error: " << err.what() << std::endl;
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
