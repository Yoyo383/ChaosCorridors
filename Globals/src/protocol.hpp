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
		INIT_PLAYER,
		UPDATE_PLAYER,
		UPDATE_BULLET,
		CLEAR_BULLETS
	};

	struct PositionInfoPacket
	{
		PacketType type = PacketType::NO_PACKET;
		char index = -1;
		sf::Vector2f position;
		float direction = 0;
	};

	std::pair<std::string, std::string> receiveKeyValue(sockets::Socket& socket);
	std::string keyValueMessage(std::string key, std::string value);

	PositionInfoPacket receivePositionInfo(sockets::Socket& socket);

	void sendPositionInfo(sockets::Socket& socket, sockets::Address& address, PositionInfoPacket packet);
}
