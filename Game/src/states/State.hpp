#pragma once
#include "SFML/Graphics.hpp"
#include "../graphics.hpp"

class StateManager;

class State {
public:
	State(StateManager& manager, sf::RenderWindow& window, TextureManager& textures) 
		: manager(manager), window(window), textures(textures) {}

	virtual void update() = 0;
	virtual void draw() = 0;

protected:
	StateManager& manager;
	sf::RenderWindow& window;
	TextureManager& textures;
};

