#pragma once
#include "SFML/Graphics.hpp"

class TextField
{
public:
	TextField(sf::Vector2f position, sf::Font& font, std::string defaultText, int maxSize, int textSize);
	std::string getText() const;

	bool contains(sf::Vector2f pos);
	void setFocus(bool focus);
	void handleInput(sf::Event event);

	void setPosition(sf::Vector2f position);

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
