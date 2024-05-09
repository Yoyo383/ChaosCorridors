#pragma once
#include "State.hpp"
#include "StateManager.hpp"
#include "sockets.hpp"

class LobbyState : public State {
public:
	LobbyState(StateManager& manager, sf::RenderWindow& window, TextureManager& textures, sockets::Socket socket);

	void update() override;
	void draw() override;

private:
	sockets::Socket socket;

	std::vector<sf::Text> playerNamesTexts;
	sf::Font font;
};
