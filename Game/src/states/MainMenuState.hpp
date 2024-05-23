#pragma once
#include "StateManager.hpp"
#include "SFML/Graphics.hpp"
#include "../ui/Button.hpp"
#include "../TextureManager.hpp"
#include "../ui/TextField.hpp"
#include "../Members.hpp"
#include <thread>

class MainMenuState : public State
{
public:
	MainMenuState(Members& members);

	void update() override;
	void draw() override;

private:
	Members& members;

	Button hostButton;
	TextField nameField;
	TextField ipField;

	sf::Sprite background;

	sf::Text errorText;

	std::thread connectThread;
	bool canConnect;
};
