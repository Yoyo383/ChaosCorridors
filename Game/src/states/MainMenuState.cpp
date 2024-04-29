#include "MainMenuState.hpp"
#include <iostream>

MainMenuState::MainMenuState(sf::RenderWindow& window, graphics::TextureManager& textures)
	: window(window),
	textures(textures),
	hostButton({ 100, 100 }, textures, "floor", "wall")
{}

void MainMenuState::update() {
	sf::Event event;

	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			window.close();
		else if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape)
				window.close();
		}
	}

	if (hostButton.isButtonClicked(window))
		std::cout << "clicked!" << std::endl;
}

void MainMenuState::draw() {
	window.clear(sf::Color::White);

	hostButton.draw(window);

	window.display();
}
