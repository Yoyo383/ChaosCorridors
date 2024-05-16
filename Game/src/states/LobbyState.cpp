#include "LobbyState.hpp"
#include "GameState.hpp"
#include "protocol.hpp"
#include <iostream>

LobbyState::LobbyState(StateManager& manager, sf::RenderWindow& window, TextureManager& textures, sockets::Socket tcpSocket, sockets::Socket udpSocket)
	: State{ manager, window, textures }, tcpSocket(tcpSocket), udpSocket(udpSocket) {
	font.loadFromFile("C:/Windows/Fonts/Arial.ttf");
	tcpSocket.setBlocking(false);
}

void LobbyState::update() {

	sf::Event event;

	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			tcpSocket.close();
			manager.quit();
		}
	}

	try {
		auto [key, value] = protocol::receiveKeyValue(tcpSocket);
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
			std::unique_ptr<GameState> gameState = std::make_unique<GameState>(manager, window, textures, tcpSocket, udpSocket);
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
