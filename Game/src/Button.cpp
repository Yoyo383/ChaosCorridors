#include "Button.hpp"
#include "graphics.hpp"

Button::Button(sf::Vector2f position, graphics::TextureManager& textures, std::string normalID, std::string clickedID)
	: normalSprite(textures[normalID]),
	clickedSprite(textures[clickedID]),
	currentSprite(&normalSprite) 
{
	normalSprite.setPosition(position);
	clickedSprite.setPosition(position);
	isClicked = false;
}

bool Button::isButtonClicked(sf::RenderWindow& window) {
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		sf::FloatRect buttonRect = currentSprite->getGlobalBounds();
		isClicked = mousePos.x > buttonRect.left && mousePos.x < buttonRect.left + buttonRect.width
			&& mousePos.y > buttonRect.top && mousePos.y < buttonRect.top + buttonRect.height;
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
