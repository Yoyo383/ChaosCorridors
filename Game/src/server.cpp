#include <iostream>
#include <map>
#include "sockets.hpp"
#include "SFML/System.hpp"
#include "server.hpp"

constexpr unsigned short PORT = 12345;

struct PlayerData {
	sf::Vector2f pos;
};

void server() {
	std::map<sockets::Address, PlayerData> players;

	sockets::Socket serverSocket(sockets::Protocol::UDP);
	serverSocket.setTimeout(0);
	serverSocket.bind({ "0.0.0.0", PORT });
	while (true) {
		try {
			auto [data, address] = serverSocket.recvFrom(1024);
			if (std::string(data.begin(), data.end()) == "connect") {
				players[address] = { { 1.5f, 1.5f } };
				std::cout << "connected" << std::endl;
			}
			else if (players.find(address) != players.end()) {
				float x = data[0], y = data[1];
				std::cout << x << ", " << y << std::endl;
			}
		}
		catch (const std::exception& err) {
			std::cout << "Error on server socket: " << err.what() << std::endl;
		}
	}

	serverSocket.close();
}
