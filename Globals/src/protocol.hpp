#pragma once
#include <tuple>
#include <string>
#include <vector>
#include <unordered_map>
#include "sockets.hpp"

namespace protocol {

	struct Position {
		float x;
		float y;
	};

	struct PlayerPositionInfo {
		char playerIndex;
		Position position;
	};

	std::tuple<std::string, std::string> receiveKeyValue(sockets::Socket& socket);
	std::string keyValueMessage(std::string key, std::string value);

	PlayerPositionInfo receivePlayer(sockets::Socket& socket);

	void sendPlayerPosition(sockets::Socket& socket, sockets::Address& address, PlayerPositionInfo packet);
}
