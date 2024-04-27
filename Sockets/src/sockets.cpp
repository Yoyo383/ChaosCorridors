#include "sockets.hpp"
#include <stdexcept>
#include <memory>

static sockaddr_in addressToRawAddress(sockets::Address address) {
	int result;
	// getting ip from string
	unsigned long ip;

	result = inet_pton(AF_INET, address.ip.c_str(), &ip);
	if (result != 1)
		throw std::exception("Error: Invalid IP.");

	// setting up the address
	sockaddr_in rawAddress;
	memset(&rawAddress, 0, sizeof(rawAddress));
	rawAddress.sin_family = AF_INET;
	rawAddress.sin_port = htons(address.port);
	rawAddress.sin_addr.s_addr = ip;

	return rawAddress;
}
static sockets::Address rawAddressToAddress(sockaddr_in rawAddress) {
	char ipBuffer[16];

	PCSTR result = inet_ntop(AF_INET, &rawAddress.sin_addr.s_addr, ipBuffer, 16);
	if (result == NULL)
		throw std::exception(("Error: " + std::to_string(WSAGetLastError())).c_str());

	std::string ip = ipBuffer;
	unsigned short port = ntohs(rawAddress.sin_port);

	sockets::Address resultAddr = { ip, port };
	return resultAddr;
}

namespace sockets {

	bool initialize() {
		// initializing WSA
		WSADATA wsaData;
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		return result == 0;
	}

	void terminate() {
		WSACleanup();
	}

	Socket::Socket(SOCKET id, Protocol protocol) {
		_socketId = id;
		_protocol = protocol;
	}

	Socket::Socket(Protocol protocol) {
		_protocol = protocol;
		int type = 0;

		if (_protocol == Protocol::TCP)
			type = SOCK_STREAM;
		else if (_protocol == Protocol::UDP)
			type = SOCK_DGRAM;

		_socketId = socket(AF_INET, type, 0);

		if (_socketId == INVALID_SOCKET) {
			throw std::exception(("Error when creating socket: " + std::to_string(WSAGetLastError())).c_str());
		}
	}

	void Socket::bind(Address address) {
		sockaddr_in bindAddress = addressToRawAddress(address);

		int result = ::bind(_socketId, (sockaddr*)&bindAddress, sizeof(bindAddress));
		if (result != 0)
			throw std::exception(("Error when binding socket: " + std::to_string(WSAGetLastError())).c_str());
	}

	void Socket::listen(int backlog) {
		int result = ::listen(_socketId, backlog);
		if (result != 0)
			throw std::exception(("Error when listening: " + std::to_string(WSAGetLastError())).c_str());
	}

	std::pair<Socket, Address> Socket::accept() {
		// address setup
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		socklen_t addrlen = sizeof(addr);


		SOCKET newId = ::accept(_socketId, (struct sockaddr*)&addr, &addrlen);
		if (newId == INVALID_SOCKET)
			throw std::exception(("Error when accepting: " + std::to_string(WSAGetLastError())).c_str());

		Socket socket(newId, Protocol::TCP);

		Address resultAddr = rawAddressToAddress(addr);
		
		return std::make_pair(socket, resultAddr);
	}

	void Socket::close() {
		int result = ::closesocket(_socketId);
		if (result == SOCKET_ERROR)
			throw std::exception(("Error when closing socket: " + std::to_string(WSAGetLastError())).c_str());
	}

	void Socket::connect(Address address) {
		sockaddr_in connectAddress = addressToRawAddress(address);

		int result = ::connect(_socketId, (sockaddr*)&connectAddress, sizeof(connectAddress));
		if (result != 0)
			throw std::exception(("Error when connecting: " + std::to_string(WSAGetLastError())).c_str());
	}

	// TCP send/recv
	int Socket::send(const char* data, int size) {
		int result = ::send(_socketId, data, size, 0);
		if (result == SOCKET_ERROR)
			throw std::exception(("Error when sending: " + std::to_string(WSAGetLastError())).c_str());
		return result;
	}
	int Socket::send(std::vector<char> data) {
		return send(data.data(), data.size());
	}
	int Socket::send(std::string data) {
		return send(data.data(), data.size());
	}

	std::vector<char> Socket::recv(int size) {
		std::vector<char> buf(size);
		int bytes = ::recv(_socketId, buf.data(), size, 0);

		if (bytes == SOCKET_ERROR)
			throw std::exception(("Error when receiving: " + std::to_string(WSAGetLastError())).c_str());

		buf.resize(bytes);
		return buf;
	}
	std::string Socket::recvString(int size) {
		std::vector<char> data = recv(size);
		return std::string(data.begin(), data.end());
	}

	// UDP send/recv
	int Socket::sendTo(const char* data, int size, Address address) {
		sockaddr_in sendAddress = addressToRawAddress(address);

		int result = ::sendto(_socketId, data, size, 0, (sockaddr*)&sendAddress, sizeof(sendAddress));
		if (result == SOCKET_ERROR)
			throw std::exception(("Error when sending: " + std::to_string(WSAGetLastError())).c_str());
		return result;
	}
	int Socket::sendTo(std::vector<char> data, Address address) {
		return sendTo(data.data(), data.size(), address);
	}
	int Socket::sendTo(std::string data, Address address) {
		return sendTo(data.data(), data.size(), address);
	}

	std::pair<std::vector<char>, Address> Socket::recvFrom(int size) {
		std::vector<char> buf(size);

		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		socklen_t addrlen = sizeof(addr);
		int bytes = ::recvfrom(_socketId, buf.data(), size, 0, (sockaddr*)&addr, &addrlen);

		if (bytes == SOCKET_ERROR)
			throw std::exception(("Error when receiving: " + std::to_string(WSAGetLastError())).c_str());

		Address resultAddress = rawAddressToAddress(addr);

		buf.resize(bytes);
		return std::make_pair(buf, resultAddress);
	}
	std::pair<std::string, Address> Socket::recvFromString(int size) {
		auto [data, address] = recvFrom(size);
		return std::make_pair(std::string(data.begin(), data.end()), address);
	}
}
