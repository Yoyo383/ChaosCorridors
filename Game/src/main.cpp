#include "sockets.hpp"
#include "states/MainMenuState.hpp"
#include "states/StateManager.hpp"
#include "Members.hpp"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

/**
 * @brief Loads all the textures.
 * @param textures The texture manager.
 */
static void loadTextures(TextureManager& textures)
{
	textures.addTexture("floor", "assets/wood.png");
	textures["floor"].setRepeated(true);
	textures.addTexture("ceiling", "assets/colorstone.png");
	textures["ceiling"].setRepeated(true);
	textures.addTexture("wall", "assets/redbrick.png");
	textures.addTexture("character", "assets/character.png");
	textures.addTexture("playButton", "assets/playButton.png");
	textures.addTexture("playButtonPressed", "assets/playButtonPressed.png");
	textures.addTexture("bullet", "assets/bullet.png");
	textures.addTexture("heart", "assets/heart.png");
	textures.addTexture("logo", "assets/logo.png");
	textures.addTexture("crosshair", "assets/crosshair.png");
}

/**
 * @brief The main function.
 * @return Exit code.
 */
int main()
{
	sockets::initialize();

	Members members;

	// creating window
	members.window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Chaos Corridors", sf::Style::Titlebar | sf::Style::Close);
	members.window.setVerticalSyncEnabled(true);

	// loading textures
	loadTextures(members.textures);

	// loading font
	if (!members.font.loadFromFile("assets/retro.ttf"))
		throw std::exception("Couldn't load font.");

	// loading main menu
	std::unique_ptr<MainMenuState> mainMenuState = std::make_unique<MainMenuState>(members);
	members.manager.addState(std::move(mainMenuState));

	// game loop
	while (members.manager.isRunning())
	{
		members.manager.changeState();
		members.manager.update();
		members.manager.draw();
	}

	sockets::shutdown();
}
