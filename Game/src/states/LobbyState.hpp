#pragma once
#include "State.hpp"
#include "StateManager.hpp"
#include "sockets.hpp"
#include "../Members.hpp"

class LobbyState : public State
{
public:
	LobbyState(Members& members);

	void update() override;
	void draw() override;

private:
	Members& members;

	std::vector<sf::Text> playerNamesTexts;
	sf::Font font;
};
