#include <iostream>
#include <unordered_map>
#include <thread>
#include "sockets.hpp"
#include "protocol.hpp"
#include "globals.hpp"
#include "maze.hpp"

constexpr unsigned short PORT = 12345;
static int count = 0;
std::vector<sockets::Socket> clientSockets;
std::vector<std::string> names;

template<typename T>
void deleteElement(std::vector<T> vector, T element) {
	vector.erase(std::remove(vector.begin(), vector.end(), element), vector.end());
}

static void broadcast(std::string message) {
	for (auto& socket : clientSockets)
		socket.send(message);
}
static void broadcast(std::vector<char> message) {
	for (auto& socket : clientSockets)
		socket.send(message);
}

static void handleClient(sockets::Socket socket, sockets::Address address) {
	clientSockets.push_back(socket);

	std::cout << address.ip << " " << address.port << std::endl;
	auto [key, value] = protocol::receiveKeyValue(socket);

	for (auto& name : names)
		socket.send("player:" + name + "\n");

	if (key == "player") {
		broadcast("player:" + value + "\n");
		names.push_back(value);
	}

	if (socket.recvString(1024) == "") {
		deleteElement(clientSockets, socket);
		deleteElement(names, value);
		socket.close();
		count--;
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
		broadcast("start:\n");
		std::vector<char> maze = mazeToVector(globals::generateMaze());
		broadcast(maze);
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
