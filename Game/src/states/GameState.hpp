#pragma once
#include "SFML/Graphics.hpp"
#include "StateManager.hpp"
#include "../player.hpp"
#include "../graphics.hpp"
#include "sockets.hpp"
#include "../Members.hpp"

struct Bullet {
	sf::Vector2f position;
	sf::Vector2f direction;
};

class GameState : public State {
public:
	GameState(Members& members);
	~GameState();

	void update() override;
	void draw() override;

	void resetMousePos();
	sf::Vector2f wasdInput();

	void drawFloorAndCeiling();
	void drawWalls();
	void drawSprite(const sf::Vector2f& characterPos, std::string texture);

private:
	Members& members;

	sockets::Address serverAddress;

	globals::MazeArr maze;

	sf::Clock deltaClock;
	float dt;

	Player player;

	sf::Vector2i fixedMousePos;
	bool isFocused;

	bool paused;

	float* zBuffer;

	std::vector<Bullet> bullets; // position, direction

	std::unordered_map<int, sf::Vector2f> players;
};
