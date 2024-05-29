#pragma once
#include "sockets.hpp"
#include "SFML/Graphics.hpp"
#include "states/StateManager.hpp"
#include "TextureManager.hpp"

// A struct to manage all global members between states.
struct Members
{
	/**
	 * @brief Creates a new Members object with the sockets initialized.
	 */
	Members() : tcpSocket(sockets::Protocol::TCP), udpSocket(sockets::Protocol::UDP), playerIndex(0) {}

	// The SFML window.
	sf::RenderWindow window;

	// The font.
	sf::Font font;

	// The state manager.
	StateManager manager;

	// The texture manager.
	TextureManager textures;

	// Socket for TCP communication.
	sockets::Socket tcpSocket;

	// Socket for UDP communication.
	sockets::Socket udpSocket;

	// Player index in the server.
	char playerIndex;
};
