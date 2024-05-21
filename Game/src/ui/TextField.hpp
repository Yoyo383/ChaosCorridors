#pragma once
#include "SFML/Graphics.hpp"

class TextField
{
public:
	TextField(sf::Vector2f position, std::string fontFilename);
	std::string getText() const;
	bool contains(sf::Vector2f pos);
	void setFocus(bool focus);
	void handleInput(sf::Event event);
	void draw(sf::RenderWindow& window) const;

private:
	sf::Font font;
	sf::Text text;
	sf::RectangleShape rect;
	bool isFocused;
};
