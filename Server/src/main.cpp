#include <iostream>
#include <unordered_map>
#include <thread>
#include "sockets.hpp"
#include "protocol.hpp"
#include "globals.hpp"
#include "maze.hpp"

constexpr unsigned short PORT = 12345;
static int count = 0;
std::vector<sockets::Socket> tcpSockets;
std::vector<sockets::Address> addresses;

std::vector<std::string> names;

template<typename T>
void deleteElement(std::vector<T> vector, T element) {
	vector.erase(std::remove(vector.begin(), vector.end(), element), vector.end());
}

static void broadcast(std::string message) {
	for (auto& socket : tcpSockets)
		socket.send(message);
}
static void broadcast(std::vector<char> message) {
	for (auto& socket : tcpSockets)
		socket.send(message);
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
			broadcast(protocol::keyValueMessage("player", value));
			names.push_back(value);
		}

		if (key == "udp") {
			udpAddress = { address.ip, (unsigned short)std::stoul(value.c_str()) };
			addresses.push_back(udpAddress);
		}

		if (key == "close") {
			deleteElement(tcpSockets, socket);
			deleteElement(names, value);
			deleteElement(addresses, udpAddress);
			socket.close();
			count--;
			closed = true;
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
	sockets::initialize();

	sockets::Socket serverSocket(sockets::Protocol::TCP);
	sockets::Socket udpSocket(sockets::Protocol::UDP);
	udpSocket.bind({ "0.0.0.0", 54321 });

	try {
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
		std::vector<char> maze = mazeToVector(globals::generateMaze());
		broadcast(maze);
		broadcastUDP(udpSocket, "hello");
		broadcastUDP(udpSocket, std::vector<char>{10, 20});
		broadcastUDP(udpSocket, "there");
		broadcastUDP(udpSocket, std::vector<char>{32, 1});
		while (count > 0) {

		}
		std::cout << "bye bye" << std::endl;
	}
	catch (std::exception& err) {
		std::cout << "Server client error: " << err.what() << std::endl;
	}

	serverSocket.close();

	sockets::terminate();
}
