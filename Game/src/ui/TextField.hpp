#pragma once
#include "SFML/Graphics.hpp"

/**
 * @brief Text field class.
 */
class TextField
{
public:
	/**
	 * @brief Creates a new TextField object.
	 * @param position The position.
	 * @param font The text font.
	 * @param defaultText The text that shows at the start of the field.
	 * @param maxSize Maximum number of characters.
	 * @param textSize Character size.
	 */
	TextField(sf::Vector2f position, sf::Font& font, std::string defaultText, int maxSize, int textSize);

	/**
	 * @brief Returns the text of the field.
	 * @return The text of the field.
	 */
	std::string getText() const;

	/**
	 * @brief Checks if a position is inside the field.
	 * @param pos The position.
	 * @return Whether the position is inside the field.
	 */
	bool contains(sf::Vector2f pos);

	/**
	 * @brief Sets whether the field is focused.
	 * @param focus Whether the field is focused.
	 */
	void setFocus(bool focus);

	/**
	 * @brief Handles keyboard input.
	 * @param event An SFML event.
	 */
	void handleInput(sf::Event event);

	/**
	 * @brief Sets the position of the field.
	 * @param position The new position.
	 */
	void setPosition(sf::Vector2f position);

	/**
	 * @brief Draws the field.
	 * @param window The window.
	 */
	void draw(sf::RenderWindow& window) const;

private:
	sf::Font& font;
	sf::Text text;
	sf::RectangleShape rect;

	int maxSize;

	std::string defaultText;
	std::string currentText;

	bool isFocused;
};
