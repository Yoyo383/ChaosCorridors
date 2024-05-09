#pragma once
#include "SFML/Graphics.hpp"
#include "StateManager.hpp"
#include "../player.hpp"
#include "../graphics.hpp"
#include "sockets.hpp"

class GameState : public State {
public:
	GameState(StateManager& manager, sf::RenderWindow& window, TextureManager& textures, sockets::Socket socket);
	~GameState();

	void update() override;
	void draw() override;

	void resetMousePos();
	sf::Vector2f wasdInput();

	void drawFloorAndCeiling();
	void drawWalls();
	void drawCharacter(const sf::Vector2f& characterPos);

private:
	sockets::Socket socket;

	MazeArr maze;

	sf::Clock deltaClock;
	float dt;

	Player player;

	sf::Vector2i fixedMousePos;
	bool isFocused;

	float* zBuffer;
};
