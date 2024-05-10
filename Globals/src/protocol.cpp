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
				std::cout << "Error in recv: " << err.what() << std::endl;
				return std::make_tuple("", "");
			}
		}

		return std::make_tuple(key, value);
	}
}
