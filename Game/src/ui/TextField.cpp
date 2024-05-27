#include "TextField.hpp"
#include "util.hpp"

TextField::TextField(sf::Vector2f position, sf::Font& font, std::string defaultText, int maxSize, int textSize) 
	: isFocused(false), font(font), defaultText(defaultText), maxSize(maxSize)
{
	rect.setSize({ (maxSize + (int)defaultText.size()) * textSize * 0.8f, textSize * 1.25f });
	rect.setOrigin(rect.getSize() / 2);
	rect.setPosition(position);
	rect.setOutlineThickness(2);
	rect.setOutlineColor(sf::Color::White);
	rect.setFillColor(sf::Color::Transparent);

	text.setFont(font);
	text.setCharacterSize(textSize);
	text.setPosition(position - rect.getSize() / 2);
	text.setFillColor(sf::Color::White);
	text.setString(defaultText);
}

std::string TextField::getText() const
{
	return currentText;
}

bool TextField::contains(sf::Vector2f pos)
{
	return rect.getGlobalBounds().contains(pos);
}

void TextField::setFocus(bool focus)
{
	isFocused = focus;
	if (isFocused)
		rect.setOutlineColor(sf::Color::Red);
	else
		rect.setOutlineColor(sf::Color::White);
}

void TextField::handleInput(sf::Event event)
{
	if (!isFocused || event.type != sf::Event::TextEntered)
		return;

	unsigned char code = static_cast<unsigned char>(event.text.unicode);

	if (code == '\b')
	{
		if (!currentText.empty())
			currentText.pop_back(); // remove last character
	}
	else if (isprint(code) && currentText.size() < maxSize)
		currentText += code;

	text.setString(defaultText + currentText);
}

void TextField::setPosition(sf::Vector2f position)
{
	rect.setPosition(position);
	text.setPosition(position - rect.getSize() / 2);
}

void TextField::draw(sf::RenderWindow& window) const
{
	window.draw(rect);
	window.draw(text);
}
