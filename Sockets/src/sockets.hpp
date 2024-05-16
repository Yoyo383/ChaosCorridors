/**
* A library to simplify networking and pack it in classes and structs.
*/
#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <vector>

namespace sockets {

	/**
	 * @brief An enum that represents the protocol of the socket - TCP or UDP.
	*/
	enum class Protocol {
		TCP,
		UDP
	};

	/**
	 * @brief A struct that represents an address (IP and port).
	*/
	struct Address {
		std::string ip;
		unsigned short port;
	};

	/**
	 * @brief Checks if two addresses are equal.
	 * @param addr1 First Address object.
	 * @param addr2 Second Address object.
	 * @return Whether addr1 is equal to addr2.
	*/
	bool operator ==(const Address& addr1, const Address& addr2);

	/**
	 * @brief Initializes the socket library. Should be called at the start of the code.
	 * @return Whether the initialization was successful.
	*/
	bool initialize();
	/**
	 * @brief Terminates the library. Should be called at the end of the code.
	*/
	void terminate();

	/**
	 * @brief Socket class.
	*/
	class Socket {
	private:
		// The ID of the socket, used for windows functions.
		SOCKET socketId;
		// The protocol of the socket.
		Protocol protocol;

		/**
		 * @brief Creates a new socket with an ID and a protocol.
		 * @param id The ID of the socket.
		 * @param protocol The protocol of the socket.
		*/
		Socket(SOCKET id, Protocol protocol);
	public:
		/**
		 * @brief Creates a new socket with a protocol.
		 * @param protocol The protocol of the socket.
		*/
		Socket(Protocol protocol);

		/**
		 * @brief Checks if two sockets are equal.
		 * @param other The other socket.
		 * @return Whether the current and other socket are equal.
		*/
		bool operator ==(const Socket& other);

		/**
		 * @brief Binds the socket to an address.
		 * @param address The address to bind.
		*/
		void bind(Address address);

		/**
		 * @brief Enable a socket to accept connections.
		 * @param The number of unaccepted connections that the system will allow before refusing new connections.
		*/
		void listen(int backlog);

		/**
		 * @brief Accepts a connection.
		 * @return A pair with a new socket object usable to send and receive data on the connection,
		 * and the address bound to the socket on the other end of the connection.
		*/
		std::pair<Socket, Address> accept();

		/**
		 * @brief Closes the socket.
		*/
		void close();

		/**
		 * @brief Connects to an address.
		 * @param address The address to connect to.
		*/
		void connect(Address address);
		
		/**
		 * @brief Sets the timeout of the socket.
		 * @param seconds Timeout in seconds.
		*/
		void setTimeout(float seconds);

		/**
		 * @brief Sets whether the socket is in blocking or non-blocking mode.
		 * @param blocking Blocking or non-blocking mode.
		*/
		void setBlocking(bool blocking);
		
#pragma region TCP send/recv
		
		/**
		 * @brief Sends data to the socket.
		 * @param data The data to send.
		 * @param size The size of the data.
		 * @return The number of bytes sent.
		*/
		int send(const char* data, int size);
		/**
		 * @brief Sends data to the socket.
		 * @param data The data to send.
		 * @return The number of bytes sent.
		*/
		int send(std::vector<char> data);
		/**
		 * @brief Sends data to the socket.
		 * @param data The data to send.
		 * @return The number of bytes sent.
		*/
		int send(std::string data);

		/**
		 * @brief Receives data from the socket.
		 * @param size The maximum amount of data to be received.
		 * @return A vector of bytes representing the data received.
		*/
		std::vector<char> recv(int size);
		/**
		 * @brief Receives data from the socket.
		 * @param size The maximum amount of data to be received.
		 * @return A string representing the data received.
		*/
		std::string recvString(int size);

#pragma endregion

#pragma region UDP send/recv

		/**
		 * @brief Sends data to an address.
		 * @param data The data to send.
		 * @param size The size of the data.
		 * @param address The address to send to.
		 * @return The number of bytes sent.
		*/
		int sendTo(const char* data, int size, Address address);
		/**
		 * @brief Sends data to an address.
		 * @param data The data to send.
		 * @param address The address to send to.
		 * @return The number of bytes sent.
		*/
		int sendTo(std::vector<char> data, Address address);
		/**
		 * @brief Sends data to an address.
		 * @param data The data to send.
		 * @param address The address to send to.
		 * @return The number of bytes sent.
		*/
		int sendTo(std::string data, Address address);

		/**
		 * @brief Receives data from the socket.
		 * @param size The maximum amount of data to be received.
		 * @return A pair with a vector of bytes representing the data received, and the address it was sent from.
		*/
		std::pair<std::vector<char>, Address> recvFrom(int size);
		/**
		 * @brief Receives data from the socket.
		 * @param size The maximum amount of data to be received.
		 * @return A pair with a string representing the data received, and the address it was sent from.
		*/
		std::pair<std::string, Address> recvFromString(int size);

#pragma endregion

	};
}