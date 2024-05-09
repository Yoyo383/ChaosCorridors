#include "LobbyState.hpp"
#include "../util.hpp"
#include <iostream>

LobbyState::LobbyState(StateManager& manager, sf::RenderWindow& window, TextureManager& textures, sockets::Socket socket)
	: State{ manager, window, textures }, socket(socket) {
	font.loadFromFile("C:/Windows/Fonts/Arial.ttf");
	socket.setBlocking(false);
}

void LobbyState::update() {

	sf::Event event;

	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			manager.quit();
	}

	try {
		auto [received, key, value] = receiveKeyValue(socket);
		if (received)
			std::cout << key << ": " << value << std::endl;
	}
	catch (...) {

	}
}

void LobbyState::draw() {
	window.clear(sf::Color::White);
	window.display();
}
