#pragma once
#include "SFML/Graphics.hpp"
#include "../TextureManager.hpp"

class Button {
private:
	sf::Sprite normalSprite;
	sf::Sprite clickedSprite;
	sf::Sprite* currentSprite;
	bool isClicked;

public:
	Button(sf::Vector2f position, TextureManager& textures, std::string normalID, std::string clickedID);

	void setSizeRelativeToWindow(sf::RenderWindow& window, float percentage);
	void setClicked(bool clicked);

	bool isButtonClicked(sf::Vector2f pos);
	void draw(sf::RenderWindow& window) const;
};