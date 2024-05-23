#include "protocol.hpp"
#include <iostream>

namespace protocol
{
	std::pair<std::string, std::string> receiveKeyValue(sockets::Socket& socket)
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

	PositionInfoPacket receivePositionInfo(sockets::Socket& socket)
	{
		try
		{
			auto [packet, address] = socket.recvFrom<PositionInfoPacket>();
			return packet;
		}
		catch (sockets::exception& err)
		{
			if (err.getErrorCode() != WSAEWOULDBLOCK)
				std::cout << "Error: " << err.what() << std::endl;
			return { PacketType::NO_PACKET, 0, { 0, 0 } };
		}
	}

	void sendPositionInfo(sockets::Socket& socket, sockets::Address& address, PositionInfoPacket packet)
	{
		socket.sendTo(packet, address);
	}
}
