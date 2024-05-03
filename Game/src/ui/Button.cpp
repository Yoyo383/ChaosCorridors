#include "Button.hpp"

Button::Button(sf::Vector2f position, TextureManager& textures, std::string normalID, std::string clickedID)
	: normalSprite(textures[normalID]),
	clickedSprite(textures[clickedID]),
	currentSprite(&normalSprite)
{
	normalSprite.setPosition(position);
	normalSprite.setOrigin(normalSprite.getLocalBounds().width / 2.0f, normalSprite.getLocalBounds().height / 2.0f);

	clickedSprite.setPosition(position);
	clickedSprite.setOrigin(clickedSprite.getLocalBounds().width / 2.0f, clickedSprite.getLocalBounds().height / 2.0f);
	isClicked = false;
}

void Button::setSizeRelativeToWindow(sf::RenderWindow& window, float percentage) {
	float scale = window.getSize().x / (1 / percentage * currentSprite->getGlobalBounds().width);
	normalSprite.setScale(scale, scale);
	clickedSprite.setScale(scale, scale);
}

void Button::setClicked(bool clicked) {
	isClicked = clicked;
	if (isClicked)
		currentSprite = &clickedSprite;
	else
		currentSprite = &normalSprite;
}

bool Button::isButtonClicked(sf::Vector2f pos) {
	sf::FloatRect buttonRect = currentSprite->getGlobalBounds();
	setClicked(buttonRect.contains(pos));
	return isClicked;
}

void Button::draw(sf::RenderWindow& window) const {
	window.draw(*currentSprite);
}
