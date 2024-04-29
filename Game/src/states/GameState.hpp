#pragma once
#include "SFML/Graphics.hpp"
#include "State.hpp"
#include "../player.hpp"
#include "../graphics.hpp"

class GameState : public State {
private:
	sf::RenderWindow& window;

	MazeArr maze;

	sf::Clock deltaClock;
	float dt;

	Player player;

	sf::Vector2i fixedMousePos;
	bool isFocused;

	graphics::TextureManager& textures;

	float* zBuffer;

public:
	GameState(sf::RenderWindow& window, graphics::TextureManager& textures);
	~GameState();

	void update() override;
	void draw() override;

	void resetMousePos();
	sf::Vector2f wasdInput();

	void drawFloorAndCeiling();
	void drawWalls();
	void drawCharacter(const sf::Vector2f& characterPos);
};

