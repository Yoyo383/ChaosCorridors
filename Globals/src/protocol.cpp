#include "protocol.hpp"
#include <iostream>

namespace protocol
{
	std::pair<std::string, std::string> receiveKeyValue(const sockets::Socket& socket)
	{
		std::string key = "", value = "";
		std::string data;
		bool toKey = true;

		while (data != "\n")
		{
			try
			{
				data = socket.recvString(1);

				if (data == ":")
					toKey = false;
				else if (data != "\n")
				{
					if (toKey)
						key += data;
					else
						value += data;
				}
			}
			catch (sockets::exception& err)
			{
				if (err.getErrorCode() != WSAEWOULDBLOCK)
					std::cout << "Error in key/value: " << err.what() << std::endl;
				return std::make_pair("", "");
			}
		}

		return std::make_pair(key, value);
	}

	std::string keyValueMessage(std::string key, std::string value)
	{
		return key + ":" + value + "\n";
	}

	Packet receivePacket(const sockets::Socket& socket)
	{
		try
		{
			auto [packet, address] = socket.recvFrom<Packet>();
			return packet;
		}
		catch (sockets::exception& err)
		{
			if (err.getErrorCode() != WSAEWOULDBLOCK)
				std::cout << "Error: " << err.what() << std::endl;
			return { PacketType::NO_PACKET, 0, { 0, 0 } };
		}
	}

	void sendPacket(const sockets::Socket& socket, const sockets::Address& address, Packet packet)
	{
		socket.sendTo(packet, address);
	}
}
