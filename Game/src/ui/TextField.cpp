#include "TextField.hpp"

TextField::TextField(sf::Vector2f position, sf::Font& font) : isFocused(false), font(font)
{
	text.setFont(font);
	text.setPosition(position);
	text.setFillColor(sf::Color::White);

	rect.setPosition(position);
	rect.setSize({ 20 * 30, 40 });
	rect.setOutlineThickness(2);
	rect.setOutlineColor(sf::Color::White);
	rect.setFillColor(sf::Color::Transparent);
}

std::string TextField::getText() const
{
	return text.getString();
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

	std::string string = text.getString();
	unsigned char code = static_cast<unsigned char>(event.text.unicode);

	if (code == '\b')
	{
		if (!string.empty())
			string.pop_back(); // remove last character
	}
	else if (isprint(code))
		string += code;

	text.setString(string);
}

void TextField::draw(sf::RenderWindow& window) const
{
	window.draw(rect);
	window.draw(text);
}
