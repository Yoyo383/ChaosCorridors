#pragma once
#include <tuple>
#include <string>
#include <vector>
#include <unordered_map>
#include "sockets.hpp"

namespace protocol {

	struct Vector2 {
		float x;
		float y;
	};

	struct PositionInfoPacket {
		char type; // 0 for new player, 1 for bullet, 2 for clearing bullets, 3 for player target position
		char index;
		Vector2 position;
		float direction; // not used for players
	};

	std::tuple<std::string, std::string> receiveKeyValue(sockets::Socket& socket);
	std::string keyValueMessage(std::string key, std::string value);

	PositionInfoPacket receivePositionInfo(sockets::Socket& socket);

	void sendPositionInfo(sockets::Socket& socket, sockets::Address& address, PositionInfoPacket packet);
}
