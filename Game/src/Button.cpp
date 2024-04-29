#include "Button.hpp"
#include "graphics.hpp"

Button::Button(sf::Vector2f position, TextureManager& textures, std::string normalID, std::string clickedID)
	: normalSprite(textures[normalID]),
	clickedSprite(textures[clickedID]),
	currentSprite(&normalSprite) 
{
	normalSprite.setPosition(position);
	clickedSprite.setPosition(position);
	isClicked = false;
}

void Button::setSizeRelativeToWindow(sf::RenderWindow& window, float percentage) {
	float scale = window.getSize().x / (1 / percentage * currentSprite->getGlobalBounds().width);
	normalSprite.setScale(scale, scale);
	clickedSprite.setScale(scale, scale);
}

bool Button::isButtonClicked(sf::RenderWindow& window) {
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		sf::FloatRect buttonRect = currentSprite->getGlobalBounds();
		isClicked = buttonRect.contains(mousePos.x, mousePos.y);
	}
	else
		isClicked = false;

	if (isClicked)
		currentSprite = &clickedSprite;
	else
		currentSprite = &normalSprite;

	return isClicked;
}

void Button::draw(sf::RenderWindow& window) const {
	window.draw(*currentSprite);
}
