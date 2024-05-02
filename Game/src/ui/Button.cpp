#include "Button.hpp"
#include "../graphics.hpp"

Button::Button(sf::Vector2f position, TextureManager& textures, std::string textureID)
	: sprite(textures[textureID])
{
	sprite.setPosition(position);
	sprite.setOrigin(sprite.getLocalBounds().width / 2.0f, sprite.getLocalBounds().height / 2.0f);
}

void Button::setSizeRelativeToWindow(sf::RenderWindow& window, float percentage) {
	float scale = window.getSize().x / (1 / percentage * sprite.getGlobalBounds().width);
	sprite.setScale(scale, scale);
}

bool Button::isButtonClicked(sf::RenderWindow& window) {
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		sf::FloatRect buttonRect = sprite.getGlobalBounds();
		return buttonRect.contains(mousePos.x, mousePos.y);
	}
	return false;
}

void Button::draw(sf::RenderWindow& window) const {
	window.draw(sprite);
}
