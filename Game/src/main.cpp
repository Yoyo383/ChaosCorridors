#include <math.h>
#include <iostream>
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "util.hpp"
#include "maze.hpp"
#include "player.hpp"
#include "graphics.hpp"

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

	sf::RenderWindow window(sf::VideoMode(1000, 800), "Yay window!");
	window.setMouseCursorVisible(false);

	MazeArr maze = generateMaze();

	sf::Clock deltaClock;
	float dt;

	Player player;

	const sf::Vector2i FIXED_MOUSE_POS = { (int)window.getSize().x / 2, (int)window.getSize().y / 2 };
	bool isFocused = true;

	sf::Mouse::setPosition(FIXED_MOUSE_POS, window);

	graphics::Textures textures = graphics::loadTextures();

	sf::Vector2f characterPos = { 1.5f, 1.5f };

	float* zBuffer = new float[window.getSize().x + 1];


	while (window.isOpen()) {
		dt = deltaClock.restart().asSeconds();
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape)
					window.close();
			}
			else if (event.type == sf::Event::LostFocus)
				isFocused = false;
			else if (event.type == sf::Event::GainedFocus)
				isFocused = true;
		}

		// setting player's direction according to mouse
		if (isFocused) {
			player.setDirection(window, FIXED_MOUSE_POS, dt);
			resetMousePos(window, FIXED_MOUSE_POS);
		}

		// getting input
		sf::Vector2f wasd;

		if (isFocused)
			wasd = wasdInput();

		player.calculateVelocity(wasd, dt);
		player.checkCollision(maze);
		player.move();


		// start drawing

		window.clear(sf::Color::Black);

		graphics::drawFloorAndCeiling(window, player, textures);
		graphics::drawWalls(window, player, maze, zBuffer, textures);
		graphics::drawCharacter(window, player, characterPos, zBuffer, textures);

		window.display();
	}

	delete[] zBuffer;

}
