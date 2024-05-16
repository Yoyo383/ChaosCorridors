#pragma once
#include <tuple>
#include <string>
#include <unordered_map>
#include "sockets.hpp"

namespace protocol {

	struct Position {
		float x;
		float y;
	};

	std::tuple<std::string, std::string> receiveKeyValue(sockets::Socket& socket);
	std::string keyValueMessage(std::string key, std::string value);
	std::unordered_map<std::string, Position> receivePlayerPositions(sockets::Socket& socket, int playerCount, sockets::Address& serverAddress);
}
