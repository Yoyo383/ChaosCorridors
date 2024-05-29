#pragma once
#include "State.hpp"
#include "StateManager.hpp"
#include "sockets.hpp"
#include "../Members.hpp"

class LobbyState : public State
{
public:
	LobbyState(Members& members, std::string ip);

	void update() override;
	void draw() override;

private:
	Members& members;
	std::string ip;

	sf::Text lobbyText;
	sf::Text statusText;

	bool isFocused;

	std::vector<sf::Text> playerNamesTexts;
};
