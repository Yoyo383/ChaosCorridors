/**
* A library to simplify networking and pack it in classes and structs.
*/
#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <vector>

/**
 * @brief Turns an variable to a byte vector.
 * @tparam T The variable type.
 * @param obj The variable.
 * @return A byte vector of the variable's data.
*/
template<typename T> std::vector<char> toBytes(T obj)
{
	char* ptr = reinterpret_cast<char*>(&obj);
	return std::vector<char>(ptr, ptr + sizeof(obj));
}

/**
 * @brief Turns a vector of bytes to a variable.
 * @tparam T The variable type.
 * @param bytes The byte vector.
 * @return A variable of type T that has the data of the bytes.
*/
template<typename T> T toVariable(std::vector<char> bytes)
{
	return *reinterpret_cast<T*>(&bytes[0]);
}


namespace sockets
{

	/**
	 * @brief An enum that represents the protocol of the socket - TCP or UDP.
	*/
	enum class Protocol
	{
		TCP,
		UDP
	};

	/**
	 * @brief A struct that represents an address (IP and port).
	*/
	struct Address
	{
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
	 * @brief Shuts down the library. Should be called at the end of the code.
	*/
	void shutdown();

	/**
	 * @brief Socket class.
	*/
	class Socket
	{
	private:
		// The ID of the socket, used for windows functions.
		SOCKET socketId;

		/**
		 * @brief Creates a new socket with an ID
		 * @param id The ID of the socket.
		*/
		Socket(SOCKET id);
	public:
		/**
		 * @brief Creates a new socket with a protocol.
		 * @param protocol The protocol of the socket.
		*/
		Socket(Protocol protocol);

		/**
		 * @brief Creates a socket that can't do anything.
		*/
		Socket();

		/**
		 * @brief Returns the ID of the socket.
		 * @return The ID of the socket.
		*/
		SOCKET getID() const;

		Address getSocketName() const;

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
		 * @brief Sends a variable to the socket.
		 * @tparam T The type of the variable.
		 * @param obj The variable.
		 * @return The number of bytes sent.
		*/
		template<typename T> int send(T obj)
		{
			std::vector<char> bytes = toBytes<T>(obj);
			return send(bytes);
		}
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
		 * @brief Receives a variable from the socket.
		 * @tparam T The type of the variable.
		 * @return The variable.
		*/
		template<typename T> T recv()
		{
			std::vector<char> data = recv(sizeof(T));
			return toVariable<T>(data);
		}
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
		 * @brief Sends a variable to an address.
		 * @tparam T The type of the variable.
		 * @param obj The variable.
		 * @param address The address to send to.
		 * @return The number of bytes sent.
		*/
		template<typename T> int sendTo(T obj, Address address)
		{
			std::vector<char> data = toBytes<T>(obj);
			return sendTo(data, address);
		}
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
		 * @brief Receives a variable from the socket.
		 * @tparam T The type of the variable.
		 * @return A pair of the variable received and the address it was sent from.
		*/
		template<typename T> std::pair<T, Address> recvFrom()
		{
			auto [data, address] = recvFrom(sizeof(T));
			return std::make_pair(toVariable<T>(data), address);
		}
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

	/**
	 * @brief Checks if two sockets are equal.
	 * @param other The other socket.
	 * @return Whether the current and other socket are equal.
	*/
	bool operator ==(const Socket& sock1, const Socket& sock2);

}