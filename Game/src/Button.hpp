#pragma once
#include "SFML/Graphics.hpp"
#include "graphics.hpp"

class Button {
private:
	sf::Sprite normalSprite;
	sf::Sprite clickedSprite;
	sf::Sprite *currentSprite;
	bool isClicked;

public:
	Button(sf::Vector2f position, graphics::TextureManager& textures, std::string normalID, std::string clickedID);
	bool isButtonClicked(sf::RenderWindow& window);
	void draw(sf::RenderWindow& window) const;
};
