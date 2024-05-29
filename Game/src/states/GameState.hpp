#pragma once
#include "SFML/Graphics.hpp"
#include "StateManager.hpp"
#include "Player.hpp"
#include "../TextureManager.hpp"
#include "sockets.hpp"
#include "../Members.hpp"

// Represents a casted ray.
struct Ray
{
	bool isHit;
	bool verticalHit;
	float distance;
	float hitCoord;
};

/**
 * @brief Game state.
 */
class GameState : public State
{
public:
	/**
	 * @brief Creates a new GameState object.
	 * @param members The members.
	 * @param isFocused Is the window focused.
	 * @param ip The server IP.
	 */
	GameState(Members& members, bool isFocused, std::string ip);

	/**
	 * @brief Updates the state.
	 */
	void update() override;

	/**
	 * @brief Draws the state.
	 */
	void draw() override;

	/**
	 * @brief The function raycasts from the player in a certain direction and finds a collision with the world.
	 * It uses the DDA algorithm from this video: https://youtu.be/NbSee-XM7WA
	 * @param angle The angle of the ray.
	 * @return Ray object representing the ray.
	 */
	Ray raycast(float angle);

	/**
	 * @brief Resets the mouse position to the center of the screen.
	 */
	void resetMousePos();

	/**
	 * @brief Receives input from WASD keys and puts them in a vector2.
	 * @return A vector representing the input along the axis.
	 */
	sf::Vector2f wasdInput();

	/**
	 * @brief Sets the player direction according to the mouse.
	 */
	void setPlayerDirection();

	/**
	 * @brief Sends a new bullet to the server.
	 */
	void shootBullet();

	/**
	 * @brief Receives packets on UDP and process them.
	 */
	void receiveUDP();

	/**
	 * @brief Receives packets on TCP and process them.
	 */
	void receiveTCP();

	/**
	 * @brief Moves all players by a bit to their updated positions.
	 */
	void movePlayers();

	/**
	 * @brief Sends the player's position to the server (UDP).
	 */
	void sendPosition();

	/**
	 * @brief Draws the floor and ceiling.
	 */
	void drawFloorAndCeiling();

	/**
	 * @brief Draws the walls.
	 */
	void drawWalls();

	/**
	 * @brief Draws a sprite.
	 * @param position The position of the sprite in the world.
	 * @param texture The ID of the sprite's texture.
	 */
	void drawSprite(sf::Vector2f position, std::string texture);

private:
	Members& members;

	sockets::Address serverAddressUDP;

	globals::MazeArr maze;

	sf::Clock deltaClock;
	float dt;

	const int NUMBER_OF_TICKS = 30;
	float elapsedTime;

	int timer;

	int score;

	Player player;

	sf::Sprite heartSprite;
	sf::Text timerText;
	sf::Text scoreText;

	sf::Sprite crosshair;

	sf::Vector2i centerScreenPos;
	bool isFocused;
	bool paused;

	std::vector<float> zBuffer;

	std::unordered_map<int, sf::Vector2f> bullets;

	std::unordered_map<int, sf::Vector2f> players;
	std::unordered_map<int, sf::Vector2f> targetPlayerPositions;
};
