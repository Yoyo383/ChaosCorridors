#pragma once
#include "SFML/Graphics.hpp"
#include "StateManager.hpp"
#include "Player.hpp"
#include "../TextureManager.hpp"
#include "sockets.hpp"
#include "../Members.hpp"

struct Ray
{
	bool isHit;
	bool verticalHit;
	float distance;
	float hitCoord;
};

class GameState : public State
{
public:
	GameState(Members& members);
	~GameState();

	void update() override;
	void draw() override;

	/**
	* @brief The function raycasts from the player in a certain direction and finds a collision with the world.
	* It uses the DDA algorithm from this video: https://youtu.be/NbSee-XM7WA
	* @param angle The angle of the ray.
	* @return A Ray object representing the ray.
	*/
	Ray raycast(float angle);

	void resetMousePos();
	sf::Vector2f wasdInput();

	void setPlayerDirection();

	void drawFloorAndCeiling();
	void drawWalls();
	void drawSprite(const sf::Vector2f& characterPos, std::string texture);

private:
	Members& members;

	sockets::Address serverAddress;

	globals::MazeArr maze;

	sf::Clock deltaClock;
	float dt;

	float elapsedTime;

	Player player;

	sf::Vector2i fixedMousePos;
	bool isFocused;

	bool paused;

	float* zBuffer;

	std::unordered_map<int, sf::Vector2f> bullets;

	std::unordered_map<int, sf::Vector2f> players;
	std::unordered_map<int, sf::Vector2f> targetPlayerPositions;
};
