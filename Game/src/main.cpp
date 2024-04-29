#include <math.h>
#include <iostream>
#include "sockets.hpp"
#include "states/GameState.hpp"
#include "states/MainMenuState.hpp"
#include "states/StateManager.hpp"

constexpr unsigned short PORT = 12345;

static sf::Vector2f wasdInput() {
	return {
		(float)(sf::Keyboard::isKeyPressed(sf::Keyboard::W) - sf::Keyboard::isKeyPressed(sf::Keyboard::S)),
		(float)(sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	};
}

static void resetMousePos(sf::RenderWindow& window, sf::Vector2i fixedMousePos) {
	sf::Mouse::setPosition(fixedMousePos, window);
}

int main() {
	srand(time(NULL));
	sockets::initialize();

	sf::RenderWindow window(sf::VideoMode(1000, 800), "Yay window!");
	TextureManager textures;
	textures.addTexture("floor", "assets/wood.png");
	textures["floor"].setRepeated(true);
	textures.addTexture("ceiling", "assets/colorstone.png");
	textures["ceiling"].setRepeated(true);
	textures.addTexture("wall", "assets/redbrick.png");
	textures.addTexture("character", "assets/character.png");
	textures.addTexture("buttonNormal", "assets/buttonNormal.png");
	textures.addTexture("buttonClicked", "assets/buttonClicked.png");

	StateManager stateManager;

	std::unique_ptr<MainMenuState> mainMenuState(new MainMenuState(stateManager, window, textures));
	stateManager.setNextState(std::move(mainMenuState));

	while (stateManager.isRunning()) {
		stateManager.changeState();
		stateManager.update();
		stateManager.draw();
	}

	sockets::terminate();
}
