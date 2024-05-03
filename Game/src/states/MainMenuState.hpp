#pragma once
#include "StateManager.hpp"
#include "SFML/Graphics.hpp"
#include "../ui/Button.hpp"
#include "../graphics.hpp"
#include "../ui/TextField.hpp"

class MainMenuState : public State {
private:
	Button hostButton;
	TextField nameField;

public:
	MainMenuState(StateManager& manager, sf::RenderWindow& window, TextureManager& textures);

	void update() override;
	void draw() override;
};
