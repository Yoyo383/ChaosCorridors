#pragma once
#include "State.hpp"
#include "StateManager.hpp"
#include "sockets.hpp"

class LobbyState : public State {
public:
	LobbyState(StateManager& manager, sf::RenderWindow& window, TextureManager& textures, sockets::Socket tcpSocket, sockets::Socket udpSocket);

	void update() override;
	void draw() override;

private:
	sockets::Socket tcpSocket;
	sockets::Socket udpSocket;

	std::vector<sf::Text> playerNamesTexts;
	sf::Font font;
};
