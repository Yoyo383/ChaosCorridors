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

	std::unordered_map<std::string, Position> receivePlayerPositions(sockets::Socket& socket, int playerCount, sockets::Address& serverAddress) {
		float x = 0, y = 0;
		std::string name;
		sockets::Address address;

		std::unordered_map<std::string, Position> map;

		try {
			for (int i = 0; i < playerCount; i++) {
				name = "";
				auto rawData = socket.recvFromString(1024);
				name = rawData.first;
				address = rawData.second;

				if (address == serverAddress) {
					auto [position, address] = socket.recvFrom(2);
					if (address == serverAddress)
						map[name] = { (float)position[0], (float)position[1] };
				}
			}
		}
		catch (std::exception& err) {
			if (err.what() == std::to_string(WSAEWOULDBLOCK))
				return std::unordered_map<std::string, Position>{};
			std::cout << err.what() << std::endl;
			return std::unordered_map<std::string, Position>{};
		}

		return map;
	}
}
