#pragma once

#include "State.hpp"
#include "../Members.hpp"

/**
 * @brief End state.
 */
class EndState : public State
{
public:
	/**
	 * @brief Creates a new EndState object.
	 * @param members The members.
	 * @param wonString String that says who won the game.
	 */
	EndState(Members& members, std::string wonString);

	/**
	 * @brief Updates the state.
	 */
	void update() override;

	/**
	 * @brief Draws the state.
	 */
	void draw() override;

private:
	Members& members;
	std::string wonString;

	sf::Text wonText;
};
