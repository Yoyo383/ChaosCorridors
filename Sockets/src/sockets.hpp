#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <vector>

namespace sockets {

	enum class Protocol {
		TCP,
		UDP
	};

	struct Address {
		std::string ip;
		unsigned short port;
	};

	bool initialize();
	void exit();

	class Socket {
	private:
		SOCKET _socketId;
		Protocol _protocol;

		Socket(SOCKET id, Protocol protocol);
	public:
		Socket(Protocol protocol);

		void bind(Address address);
		void listen(int backlog);
		std::pair<Socket, Address> accept();
		void close();
		void connect(Address address);
		
		int send(const char* data, int size);
		int send(std::vector<char> data);
		int send(std::string data);

		std::vector<char> recv(int size);
		std::string recvString(int size);
	};
}