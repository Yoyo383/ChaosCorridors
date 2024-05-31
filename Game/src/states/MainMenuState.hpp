#pragma once
#include "StateManager.hpp"
#include "SFML/Graphics.hpp"
#include "../ui/Button.hpp"
#include "../TextureManager.hpp"
#include "../ui/TextField.hpp"
#include "../Members.hpp"
#include <future>

/**
 * @brief Main menu state.
 */
class MainMenuState : public State
{
public:
	/**
	 * @brief Creates a new main menu.
	 * @param members The members.
	 */
	MainMenuState(Members& members);

	/**
	 * @brief Updates the state.
	 */
	void update() override;

	/**
	 * @brief Draws the state.
	 */
	void draw() override;

	/**
	 * @brief Sends and receives initial information from the server and switches to lobby state.
	 */
	void startConnection();

	/**
	 * @brief Handles button press.
	 */
	void handleButtonPress();

	/**
	 * @brief Tries connecting to the server.
	 * @return Whether the connection succeeded.
	 */
	bool connectToServer();

private:
	Members& members;

	Button hostButton;
	TextField nameField;
	TextField ipField;

	sf::Sprite logo;

	sf::Text statusText;

	std::string ip;

	bool isConnected;
};
