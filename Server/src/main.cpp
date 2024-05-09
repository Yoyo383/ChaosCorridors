#include <iostream>
#include <map>
#include <thread>
#include "sockets.hpp"

constexpr unsigned short PORT = 12345;

static int count = 0;

std::vector<sockets::Socket> clientSockets;


struct PlayerData {
	float x, y;
};

static void handleClient(sockets::Socket socket, sockets::Address address) {
	clientSockets.push_back(socket);

	std::cout << address.ip << " " << address.port << std::endl;
	std::string name = socket.recvString(1024);
	std::cout << name << std::endl;
	socket.send("hello:there\n");

	if (socket.recvString(1024) == "disconnect") {
		clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), socket), clientSockets.end());
		socket.close();
		count--;
	}
}

void main() {
	sockets::initialize();

	std::map<sockets::Address, PlayerData> players;

	sockets::Socket serverSocket(sockets::Protocol::TCP);
	try {
		serverSocket.bind({ "0.0.0.0", PORT });
		serverSocket.listen(4);

		while (count < 2) {
			auto [clientSocket, clientAddress] = serverSocket.accept();
			std::thread thread(handleClient, clientSocket, clientAddress);
			thread.detach();
			count++;
		}

		std::cout << "start!" << std::endl;
	}
	catch (std::exception& err) {
		std::cout << "Server client error: " << err.what() << std::endl;
	}

	serverSocket.close();

	sockets::terminate();
}
