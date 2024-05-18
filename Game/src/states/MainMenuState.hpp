#pragma once
#include "StateManager.hpp"
#include "SFML/Graphics.hpp"
#include "../ui/Button.hpp"
#include "../graphics.hpp"
#include "../ui/TextField.hpp"
#include "../Members.hpp"

class MainMenuState : public State {
public:
	MainMenuState(Members& members);

	void update() override;
	void draw() override;

private:
	Members& members;

	Button hostButton;
	TextField nameField;
};
