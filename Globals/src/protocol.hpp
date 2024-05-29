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

	/**
	 * @brief Receives a key-value pair.
	 * @param tcpSocket The socket to receive from.
	 * @return A pair of key and value. If an exception occurs, the key and the value will be empty strings.
	 */
	std::pair<std::string, std::string> receiveKeyValue(const sockets::Socket& tcpSocket);

	/**
	 * @brief Creates a key-value message.
	 * @param key The key.
	 * @param value The value.
	 * @return Key-value message.
	 */
	std::string keyValueMessage(std::string key, std::string value);

	/**
	 * @brief Receives a Packet.
	 * @param udpSocket The socket to receive from.
	 * @return 
	 */
	Packet receivePacket(const sockets::Socket& udpSocket);
	
	/**
	 * @brief Sends a packet to an address.
	 * @param udpSocket The socket. 
	 * @param address The address.
	 * @param packet The packet.
	 */
	void sendPacket(const sockets::Socket& udpSocket, const sockets::Address& address, Packet packet);
}
