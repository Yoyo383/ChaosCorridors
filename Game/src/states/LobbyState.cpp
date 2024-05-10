#include "LobbyState.hpp"
#include "GameState.hpp"
#include "protocol.hpp"
#include <iostream>

LobbyState::LobbyState(StateManager& manager, sf::RenderWindow& window, TextureManager& textures, sockets::Socket socket)
	: State{ manager, window, textures }, socket(socket) {
	font.loadFromFile("C:/Windows/Fonts/Arial.ttf");
	socket.setBlocking(false);
}

void LobbyState::update() {

	sf::Event event;

	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			socket.close();
			manager.quit();
		}
	}

	try {
		auto [key, value] = protocol::receiveKeyValue(socket);
		if (key == "player") {
			sf::Text text;
			text.setFont(font);
			text.setCharacterSize(90);
			text.setPosition(0, playerNamesTexts.size() * 90);
			text.setString(value);
			text.setFillColor(sf::Color::Black);
			playerNamesTexts.push_back(text);
		}
		else if (key == "start") {
			std::unique_ptr<GameState> gameState = std::make_unique<GameState>(manager, window, textures, socket);
			manager.setState(std::move(gameState));
		}
	}
	catch (...) {

	}
}

void LobbyState::draw() {
	window.clear(sf::Color::White);
	for (auto& text : playerNamesTexts) {
		window.draw(text);
	}
	window.display();
}
