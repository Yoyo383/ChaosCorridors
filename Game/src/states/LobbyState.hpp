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

	bool isFocused;

	std::vector<sf::Text> playerNamesTexts;
};
