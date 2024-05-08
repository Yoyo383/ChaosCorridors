#include <math.h>
#include <iostream>
#include "sockets.hpp"
#include "states/GameState.hpp"
#include "states/MainMenuState.hpp"
#include "states/StateManager.hpp"

constexpr unsigned short PORT = 12345;

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
	textures.addTexture("hostButton", "assets/hostButton.png");
	textures.addTexture("buttonNormal", "assets/buttonNormal.png");

	StateManager stateManager;
	
	std::unique_ptr<MainMenuState> mainMenuState = std::make_unique<MainMenuState>(stateManager, window, textures);
	stateManager.setNextState(std::move(mainMenuState));

	while (stateManager.isRunning()) {
		stateManager.changeState();
		stateManager.update();
		stateManager.draw();
	}

	sockets::terminate();
}
