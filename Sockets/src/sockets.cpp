#include "sockets.hpp"
#include <stdexcept>
#include <memory>
#include <iostream>

namespace sockets
{
	static sockaddr_in addressToRawAddress(Address address)
	{
		int result;
		// getting ip from string
		unsigned long ip = 0;
		
		result = inet_pton(AF_INET, address.ip.c_str(), &ip);
		if (result == -1)
			throw exception(WSAGetLastError());
		if (result == 0)
			throw exception("Invalid IP.");

		// setting up the address
		sockaddr_in rawAddress{};
		rawAddress.sin_family = AF_INET;
		rawAddress.sin_port = htons(address.port);
		rawAddress.sin_addr.s_addr = ip;

		return rawAddress;
	}
	static Address rawAddressToAddress(sockaddr_in rawAddress)
	{
		char ipBuffer[16];

		PCSTR result = inet_ntop(AF_INET, &rawAddress.sin_addr.s_addr, ipBuffer, 16);
		if (result == NULL)
			throw exception(WSAGetLastError());

		std::string ip = ipBuffer;
		unsigned short port = ntohs(rawAddress.sin_port);

		Address resultAddr = { ip, port };
		return resultAddr;
	}

	exception::exception(int errorCode) : errorCode(errorCode)
	{
		LPTSTR message = nullptr;
		// set error message
		int hello = FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, 
			errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)&message,
			0, 
			NULL
		);
		std::wstring tempMessage(message);
		std::string error(tempMessage.begin(), tempMessage.end());
		// remove new line
		error.pop_back();
		error.pop_back();
		errorMessage = "[Error " + std::to_string(errorCode) + "] " + error;

		LocalFree(message);
	}

	exception::exception(std::string errorMessage) : errorMessage(errorMessage), errorCode(0) { }

	const char* exception::what() const
	{
		return errorMessage.c_str();
	}

	const int exception::getErrorCode() const
	{
		return errorCode;
	}

	bool operator ==(const sockets::Address& addr1, const sockets::Address& addr2)
	{
		return addr1.port == addr2.port && addr1.ip == addr2.ip;
	}

	bool operator==(const Socket& sock1, const Socket& sock2)
	{
		return sock1.getID() == sock2.getID();
	}

	void initialize()
	{
		// initializing WSA
		WSADATA wsaData;
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0)
			throw exception(result);
	}

	void shutdown()
	{
		WSACleanup();
	}

	Socket::Socket(SOCKET id) : socketId(id), timeoutSeconds(2)
	{
		setTimeout(timeoutSeconds);
	}

	Socket::Socket() : socketId(0), timeoutSeconds(0) { }

	Socket::Socket(Protocol protocol) : timeoutSeconds(2)
	{
		int type = 0;

		if (protocol == Protocol::TCP)
			type = SOCK_STREAM;
		else if (protocol == Protocol::UDP)
			type = SOCK_DGRAM;

		socketId = socket(AF_INET, type, 0);

		if (socketId == INVALID_SOCKET)
			throw exception(WSAGetLastError());

		setTimeout(timeoutSeconds);
	}

	SOCKET Socket::getID() const
	{
		return socketId;
	}

	Address Socket::getSocketName() const
	{
		struct sockaddr_in sin {};
		socklen_t len = sizeof(sin);
		if (getsockname(socketId, (struct sockaddr*)&sin, &len) == -1)
			throw exception(WSAGetLastError());
		
		return rawAddressToAddress(sin);
	}

	void Socket::bind(Address address) const
	{
		sockaddr_in bindAddress = addressToRawAddress(address);

		int result = ::bind(socketId, (sockaddr*)&bindAddress, sizeof(bindAddress));
		if (result != 0)
			throw exception(WSAGetLastError());
	}

	void Socket::listen(int backlog) const
	{
		int result = ::listen(socketId, backlog);
		if (result != 0)
			throw exception(WSAGetLastError());
	}

	std::pair<Socket, Address> Socket::accept() const
	{
		// address setup
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		socklen_t addrlen = sizeof(addr);


		SOCKET newId = ::accept(socketId, (struct sockaddr*)&addr, &addrlen);
		if (newId == INVALID_SOCKET)
			throw exception(WSAGetLastError());

		Socket socket(newId);

		Address resultAddr = rawAddressToAddress(addr);

		return std::make_pair(socket, resultAddr);
	}

	void Socket::close() const
	{
		int result = ::closesocket(socketId);
		if (result == SOCKET_ERROR)
			throw exception(WSAGetLastError());
	}

	void Socket::connect(Address address) const
	{
		setBlocking(false);

		// starting connection
		sockaddr_in connectAddress = addressToRawAddress(address);

		int result = ::connect(socketId, (sockaddr*)&connectAddress, sizeof(connectAddress));
		if (result != 0)
		{
			int error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
				throw exception(error);
		}

		// using select to wait for connection to establish
		fd_set writefds{};
		FD_ZERO(&writefds);
		FD_SET(socketId, &writefds);

		struct timeval tv {};
		tv.tv_sec = timeoutSeconds;
		tv.tv_usec = fmodf(timeoutSeconds, 1) * 1000000;

		result = select(0, NULL, &writefds, NULL, &tv);
		if (result > 0 && FD_ISSET(socketId, &writefds))
		{
			int so_error{};
			int len = sizeof(so_error);
			getsockopt(socketId, SOL_SOCKET, SO_ERROR, (char*)so_error, &len);
			if (so_error == 0)
				setBlocking(true);
			else
				throw exception(so_error);
		}
		else if (result == 0)
		{
			close();
			throw exception("Socket timed out");
		}
		else
		{
			close();
			throw exception("Unknown error");
		}
	}

	void Socket::setTimeout(float seconds)
	{
		timeoutSeconds = seconds;
		unsigned long milliseconds = seconds * 1000;
		setsockopt(socketId, SOL_SOCKET, SO_RCVTIMEO, (char*)&milliseconds, sizeof(milliseconds));
	}

	void Socket::setBlocking(bool blocking) const
	{
		unsigned long mode = !blocking;
		ioctlsocket(socketId, FIONBIO, &mode);
	}

	// TCP send/recv
	int Socket::send(const char* data, int size) const
	{
		int result = ::send(socketId, data, size, 0);
		if (result == SOCKET_ERROR)
			throw exception(WSAGetLastError());
		return result;
	}
	int Socket::send(std::vector<char> data) const
	{
		return send(data.data(), data.size());
	}
	int Socket::send(std::string data) const
	{
		return send(data.data(), data.size());
	}

	std::vector<char> Socket::recv(int size) const
	{
		std::vector<char> buf(size);
		int bytes = ::recv(socketId, buf.data(), size, 0);

		if (bytes == SOCKET_ERROR)
			throw exception(WSAGetLastError());

		buf.resize(bytes);
		return buf;
	}
	std::string Socket::recvString(int size) const
	{
		std::vector<char> data = recv(size);
		return std::string(data.begin(), data.end());
	}

	// UDP send/recv
	int Socket::sendTo(const char* data, int size, Address address) const
	{
		sockaddr_in sendAddress = addressToRawAddress(address);

		int result = ::sendto(socketId, data, size, 0, (sockaddr*)&sendAddress, sizeof(sendAddress));
		if (result == SOCKET_ERROR)
			throw exception(WSAGetLastError());
		return result;
	}
	int Socket::sendTo(std::vector<char> data, Address address) const
	{
		return sendTo(data.data(), data.size(), address);
	}
	int Socket::sendTo(std::string data, Address address) const
	{
		return sendTo(data.data(), data.size(), address);
	}

	std::pair<std::vector<char>, Address> Socket::recvFrom(int size) const
	{
		std::vector<char> buf(size);

		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		socklen_t addrlen = sizeof(addr);
		int bytes = ::recvfrom(socketId, buf.data(), size, 0, (sockaddr*)&addr, &addrlen);

		if (bytes == SOCKET_ERROR)
			throw exception(WSAGetLastError());

		Address resultAddress = rawAddressToAddress(addr);

		buf.resize(bytes);
		return std::make_pair(buf, resultAddress);
	}
	std::pair<std::string, Address> Socket::recvFromString(int size) const
	{
		auto [data, address] = recvFrom(size);
		return std::make_pair(std::string(data.begin(), data.end()), address);
	}
}
