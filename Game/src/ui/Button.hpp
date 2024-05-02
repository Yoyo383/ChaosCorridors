#pragma once
#include "SFML/Graphics.hpp"
#include "../graphics.hpp"

class Button {
public:
	Button(sf::Vector2f position, TextureManager& textures, std::string textureID);

	void setSizeRelativeToWindow(sf::RenderWindow& window, float percentage);

	bool isButtonClicked(sf::RenderWindow& window);
	void draw(sf::RenderWindow& window) const;

private:
	sf::Sprite sprite;
};