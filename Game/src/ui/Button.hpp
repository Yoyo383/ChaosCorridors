#pragma once
#include "SFML/Graphics.hpp"
#include "../TextureManager.hpp"

/**
 * @brief Button class.
 */
class Button
{
public:
	/**
	 * @brief Creates a new Button object.
	 * @param position The position of the button.
	 * @param textures The texture manager.
	 * @param normalID Normal button texture ID.
	 * @param clickedID Clicked button texture ID.
	 */
	Button(sf::Vector2f position, TextureManager& textures, std::string normalID, std::string clickedID);

	/**
	 * @brief Sets the button size relative to the window height.
	 * @param window The window.
	 * @param percentage How much of the window height the button takes.
	 */
	void setSizeRelativeToWindow(sf::RenderWindow& window, float percentage);

	/**
	 * @brief Sets whether the button is clicked.
	 * @param clicked Whether the button is clicked or not.
	 */
	void setClicked(bool clicked);

	/**
	 * @brief Checks if the button is clicked and changes its sprite accordingly.
	 * @param pos The mouse position.
	 * @return Whether the button is clicked.
	 */
	bool isButtonClicked(sf::Vector2f pos);

	/**
	 * @brief Draws the button.
	 * @param window The window.
	 */
	void draw(sf::RenderWindow& window) const;

private:
	sf::Sprite normalSprite;
	sf::Sprite clickedSprite;
	sf::Sprite* currentSprite;

	bool isClicked;
};