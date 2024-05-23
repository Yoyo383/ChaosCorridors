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
	 * @brief Custom exception class.
	 */
	class exception : public std::exception
	{
	private:
		// Windows error code
		int errorCode;
		// Error message to display.
		std::string errorMessage;

	public:
		/**
		 * @brief Creates an exception with an error code and sets the message accordingly.
		 * @param errorCode The error code.
		 */
		exception(int errorCode);

		/**
		 * @brief Creates an exception with an error message and error code 0.
		 * @param errorMessage The error message.
		 */
		exception(std::string errorMessage);

		/**
		 * @brief Returns a string that represents the error.
		 * @return A string that represents the error.
		 */
		const char* what() const override;

		/**
		 * @brief Returns the error code.
		 * @return The error code.
		 */
		const int getErrorCode() const;
	};

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
		unsigned short port = 0;
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
	*/
	void initialize();

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

		// How many seconds before timeout.
		float timeoutSeconds;

		/**
		 * @brief Creates a new socket with an ID and a timeout of 2 seconds.
		 * @param id The ID of the socket.
		*/
		Socket(SOCKET id);
	public:
		/**
		 * @brief Creates a new socket with a protocol and a timeout of 2 seconds.
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

		/**
		 * @brief Returns the socket's own address.
		*/
		Address getSocketName() const;

		/**
		 * @brief Binds the socket to an address.
		 * @param address The address to bind.
		*/
		void bind(Address address) const;

		/**
		 * @brief Enable a socket to accept connections.
		 * @param The number of unaccepted connections that the system will allow before refusing new connections.
		*/
		void listen(int backlog) const;

		/**
		 * @brief Accepts a connection.
		 * @return A pair with a new socket object usable to send and receive data on the connection,
		 * and the address bound to the socket on the other end of the connection.
		*/
		std::pair<Socket, Address> accept() const;

		/**
		 * @brief Closes the socket.
		*/
		void close() const;

		/**
		 * @brief Connects to an address. If the timeout is exceeded, the socket will be closed and a new one will be created.
		 * @param address The address to connect to.
		*/
		void connect(Address address) const;

		/**
		 * @brief Sets the timeout of the socket.
		 * @param seconds Timeout in seconds.
		*/
		void setTimeout(float seconds);

		/**
		 * @brief Sets whether the socket is in blocking or non-blocking mode.
		 * @param blocking Blocking or non-blocking mode.
		*/
		void setBlocking(bool blocking) const;

#pragma region TCP send/recv
		/**
		 * @brief Sends a variable to the socket.
		 * @tparam T The type of the variable.
		 * @param obj The variable.
		 * @return The number of bytes sent.
		*/
		template<typename T> int send(T obj) const
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
		int send(const char* data, int size) const;
		/**
		 * @brief Sends data to the socket.
		 * @param data The data to send.
		 * @return The number of bytes sent.
		*/
		int send(std::vector<char> data) const;
		/**
		 * @brief Sends data to the socket.
		 * @param data The data to send.
		 * @return The number of bytes sent.
		*/
		int send(std::string data) const;


		/**
		 * @brief Receives a variable from the socket.
		 * @tparam T The type of the variable.
		 * @return The variable.
		*/
		template<typename T> T recv() const
		{
			std::vector<char> data = recv(sizeof(T));
			return toVariable<T>(data);
		}
		/**
		 * @brief Receives data from the socket.
		 * @param size The maximum amount of data to be received.
		 * @return A vector of bytes representing the data received.
		*/
		std::vector<char> recv(int size) const;
		/**
		 * @brief Receives data from the socket.
		 * @param size The maximum amount of data to be received.
		 * @return A string representing the data received.
		*/
		std::string recvString(int size) const;

#pragma endregion

#pragma region UDP send/recv
		/**
		 * @brief Sends a variable to an address.
		 * @tparam T The type of the variable.
		 * @param obj The variable.
		 * @param address The address to send to.
		 * @return The number of bytes sent.
		*/
		template<typename T> int sendTo(T obj, Address address) const
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
		int sendTo(const char* data, int size, Address address) const;
		/**
		 * @brief Sends data to an address.
		 * @param data The data to send.
		 * @param address The address to send to.
		 * @return The number of bytes sent.
		*/
		int sendTo(std::vector<char> data, Address address) const;
		/**
		 * @brief Sends data to an address.
		 * @param data The data to send.
		 * @param address The address to send to.
		 * @return The number of bytes sent.
		*/
		int sendTo(std::string data, Address address) const;


		/**
		 * @brief Receives a variable from the socket.
		 * @tparam T The type of the variable.
		 * @return A pair of the variable received and the address it was sent from.
		*/
		template<typename T> std::pair<T, Address> recvFrom() const
		{
			auto [data, address] = recvFrom(sizeof(T));
			return std::make_pair(toVariable<T>(data), address);
		}
		/**
		 * @brief Receives data from the socket.
		 * @param size The maximum amount of data to be received.
		 * @return A pair with a vector of bytes representing the data received, and the address it was sent from.
		*/
		std::pair<std::vector<char>, Address> recvFrom(int size) const;
		/**
		 * @brief Receives data from the socket.
		 * @param size The maximum amount of data to be received.
		 * @return A pair with a string representing the data received, and the address it was sent from.
		*/
		std::pair<std::string, Address> recvFromString(int size) const;

#pragma endregion

	};

	/**
	 * @brief Checks if two sockets are equal.
	 * @param other The other socket.
	 * @return Whether the current and other socket are equal.
	*/
	bool operator ==(const Socket& sock1, const Socket& sock2);

}