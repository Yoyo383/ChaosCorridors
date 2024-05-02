#include "MainMenuState.hpp"
#include <iostream>
#include "GameState.hpp"
#include "../server/server.hpp"
#include <thread>

MainMenuState::MainMenuState(StateManager& manager, sf::RenderWindow& window, TextureManager& textures)
	: State{ manager, window, textures },
	hostButton({ window.getSize().x / 2.0f, window.getSize().y / 2.0f }, textures, "hostButton")
{
	hostButton.setSizeRelativeToWindow(window, 0.5f);
}

void MainMenuState::update() {
	sf::Event event;

	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			manager.quit();
		else if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape)
				manager.quit();
		}
	}

	if (hostButton.isButtonClicked(window)) {
		std::unique_ptr<GameState> gameState(new GameState(manager, window, textures));
		manager.setNextState(std::move(gameState));
	}
}

void MainMenuState::draw() {
	window.clear(sf::Color::White);

	hostButton.draw(window);

	window.display();
}
