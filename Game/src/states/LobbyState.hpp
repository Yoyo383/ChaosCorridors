#pragma once
#include "State.hpp"
#include "StateManager.hpp"
#include "sockets.hpp"
#include "../Members.hpp"

/**
 * @brief Lobby state.
 */
class LobbyState : public State
{
public:
	/**
	 * @brief Creates a new LobbyState object.
	 * @param members The members.
	 * @param ip The server IP.
	 */
	LobbyState(Members& members, std::string ip);

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
	std::string ip;

	sf::Text lobbyText;
	sf::Text statusText;

	bool isFocused;

	std::vector<sf::Text> playerNamesTexts;
};
