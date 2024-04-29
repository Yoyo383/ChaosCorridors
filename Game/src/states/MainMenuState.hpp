#pragma once
#include "State.hpp"
#include "SFML/Graphics.hpp"
#include "../Button.hpp"
#include "../graphics.hpp"

class MainMenuState : public State {
private:
	sf::RenderWindow& window;
	graphics::TextureManager& textures;

	Button hostButton;

public:
	MainMenuState(sf::RenderWindow& window, graphics::TextureManager& textures);

	void update() override;
	void draw() override;
};
