#pragma once
#include <tuple>
#include <string>
#include <vector>
#include <unordered_map>
#include "sockets.hpp"
#include "SFML/System/Vector2.hpp"

namespace protocol
{

	inline const char KEY_VALUE_END = '\r';
	inline const char KEY_VALUE_SEPERATOR = ':';

	enum class PacketType : char
	{
		NO_PACKET,
		INIT_PLAYER,
		UPDATE_PLAYER,
		UPDATE_BULLET,
		CLEAR_BULLETS
	};

	struct Packet
	{
		PacketType type = PacketType::NO_PACKET;
		int index = -1;
		sf::Vector2f position;
		float direction = 0;
	};

	std::pair<std::string, std::string> receiveKeyValue(const sockets::Socket& socket);
	std::string keyValueMessage(std::string key, std::string value);

	Packet receivePacket(const sockets::Socket& socket);
	
	void sendPacket(const sockets::Socket& socket, const sockets::Address& address, Packet packet);
}
