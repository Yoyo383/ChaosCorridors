#include "protocol.hpp"
#include <iostream>

namespace protocol {

	std::tuple<std::string, std::string> receiveKeyValue(sockets::Socket& socket) {
		std::string key = "", value = "";
		std::string data;
		bool toKey = true;

		while (data != "\n") {
			try {
				data = socket.recvString(1);

				if (data == ":")
					toKey = false;
				else if (data != "\n") {
					if (toKey)
						key += data;
					else
						value += data;
				}
			}
			catch (std::exception& err) {
				if (err.what() == std::to_string(WSAEWOULDBLOCK))
					return std::make_tuple("", "");
				std::cout << "Error in key/value: " << err.what() << std::endl;
				return std::make_tuple("", "");
			}
		}

		return std::make_tuple(key, value);
	}

	std::string keyValueMessage(std::string key, std::string value) {
		return key + ":" + value + "\n";
	}

	PositionInfoPacket receivePositionInfo(sockets::Socket& socket) {
		try {
			auto [packet, address] = socket.recvFrom<PositionInfoPacket>();
			return packet;
		}
		catch (std::exception& err) {
			if (err.what() != std::to_string(WSAEWOULDBLOCK))
				std::cout << "Error: " << err.what() << std::endl;
			return { -1, 0, { 0, 0 } };
		}
	}

	void sendPositionInfo(sockets::Socket& socket, sockets::Address& address, PositionInfoPacket packet) {
		socket.sendTo(packet, address);
	}
}
