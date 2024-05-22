#pragma once
#include <tuple>
#include <string>
#include <vector>
#include <unordered_map>
#include "sockets.hpp"
#include "SFML/System/Vector2.hpp"

namespace protocol
{

	enum class PacketType : char
	{
		NO_PACKET,
		NEW_PLAYER,
		UPDATE_PLAYER,
		UPDATE_BULLET,
		CLEAR_BULLETS
	};

	struct PositionInfoPacket
	{
		PacketType type; // 0 for new player, 1 for bullet, 2 for clearing bullets, 3 for player target position
		char index;
		sf::Vector2f position;
		float direction; // not used for players
	};

	std::pair<std::string, std::string> receiveKeyValue(sockets::Socket& socket);
	std::string keyValueMessage(std::string key, std::string value);

	PositionInfoPacket receivePositionInfo(sockets::Socket& socket);

	void sendPositionInfo(sockets::Socket& socket, sockets::Address& address, PositionInfoPacket packet);
}
