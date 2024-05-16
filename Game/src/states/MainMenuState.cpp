#include "MainMenuState.hpp"
#include <iostream>
#include "LobbyState.hpp"
#include "sockets.hpp"
#include "protocol.hpp"

MainMenuState::MainMenuState(StateManager& manager, sf::RenderWindow& window, TextureManager& textures)
	: State{ manager, window, textures },
	hostButton({ window.getSize().x / 2.0f, window.getSize().y / 2.0f }, textures, "hostButton", "buttonNormal"),
	nameField({ 100, 100 }, "C:/Windows/Fonts/Arial.ttf")
{
	hostButton.setSizeRelativeToWindow(window, 0.5f);
}

void MainMenuState::update() {
	sf::Event event;

	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			manager.quit();
		else if (event.type == sf::Event::MouseButtonPressed)
			hostButton.isButtonClicked(sf::Vector2f(sf::Mouse::getPosition(window)));

		else if (event.type == sf::Event::MouseButtonReleased) {
			sf::Vector2f pos = sf::Vector2f(sf::Mouse::getPosition(window));
			nameField.setFocus(nameField.contains(pos));

			if (hostButton.isButtonClicked(pos)) {
				hostButton.setClicked(false);
				sockets::Socket socket(sockets::Protocol::TCP);
				try {
					socket.connect({ "127.0.0.1", 12345 });
					socket.send(protocol::keyValueMessage("player", nameField.getText()));

					unsigned short udpPort = (rand() % 1000) + 20000;
					sockets::Socket udpSocket(sockets::Protocol::UDP);
					udpSocket.bind({ "0.0.0.0", udpPort });

					socket.send(protocol::keyValueMessage("udp", std::to_string(udpPort)));

					std::unique_ptr<LobbyState> lobbyState = std::make_unique<LobbyState>(manager, window, textures, socket, udpSocket);
					manager.addState(std::move(lobbyState));
				}
				catch (std::exception& err) {
					std::cout << "Can't connect to server." << std::endl;
				}
			}
		}
		else {
			nameField.handleInput(event);
		}
	}
}

void MainMenuState::draw() {
	window.clear(sf::Color::White);

	hostButton.draw(window);
	nameField.draw(window);

	window.display();
}
