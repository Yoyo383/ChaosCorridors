#pragma once
#include "StateManager.hpp"
#include "SFML/Graphics.hpp"
#include "../ui/Button.hpp"
#include "../TextureManager.hpp"
#include "../ui/TextField.hpp"
#include "../Members.hpp"
#include <future>

class MainMenuState : public State
{
public:
	MainMenuState(Members& members);

	void update() override;
	void draw() override;

	bool connectToServer();

private:
	Members& members;

	Button hostButton;
	TextField nameField;
	TextField ipField;

	sf::Sprite logo;

	sf::Text statusText;

	std::string ip;

	std::future<bool> connectFuture;
	bool canConnect;
	bool doesFutureExist;
};
