#pragma once

#include "State.hpp"
#include "../Members.hpp"

class EndState : public State
{
public:
	EndState(Members& members, std::string wonString);

	void update() override;
	void draw() override;

private:
	Members& members;
	std::string wonString;

	sf::Text wonText;
};
