#pragma once
#include <tuple>
#include <string>
#include "sockets.hpp"

namespace protocol {
	std::tuple<std::string, std::string> receiveKeyValue(sockets::Socket& socket);
}
