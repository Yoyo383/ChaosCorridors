#include <math.h>
#include <iostream>
#include "sockets.hpp"
#include "states/GameState.hpp"
#include "states/MainMenuState.hpp"
#include "states/StateManager.hpp"
#include "Members.hpp"

constexpr unsigned short PORT = 12345;

int main()
{
	sockets::initialize();

	Members members;

	members.window.create(sf::VideoMode(800, 600), "Yay window!", sf::Style::Titlebar | sf::Style::Close);
	members.window.setVerticalSyncEnabled(true);

	members.textures.addTexture("floor", "assets/wood.png");
	members.textures["floor"].setRepeated(true);
	members.textures.addTexture("ceiling", "assets/colorstone.png");
	members.textures["ceiling"].setRepeated(true);
	members.textures.addTexture("wall", "assets/redbrick.png");
	members.textures.addTexture("character", "assets/character.png");
	members.textures.addTexture("playButton", "assets/playButton.png");
	members.textures.addTexture("playButtonPressed", "assets/playButtonPressed.png");
	members.textures.addTexture("bullet", "assets/bullet.png");
	members.textures.addTexture("heart", "assets/heart.png");

	if (!members.font.loadFromFile("assets/retro.ttf"))
		throw std::exception("Couldn't load font.");

	std::unique_ptr<MainMenuState> mainMenuState = std::make_unique<MainMenuState>(members);
	members.manager.addState(std::move(mainMenuState));

	while (members.manager.isRunning())
	{
		members.manager.changeState();
		members.manager.update();
		members.manager.draw();
	}

	sockets::shutdown();
}
