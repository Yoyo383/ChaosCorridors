#include "EndState.hpp"

EndState::EndState(Members& members, std::string wonString) : members(members), wonString(wonString)
{
	wonText.setFont(members.font);
	wonText.setString(wonString);
	wonText.setFillColor(sf::Color::Green);
	wonText.setCharacterSize(60);
}

void EndState::update()
{
	sf::Event event;

	while (members.window.pollEvent(event))
		if (event.type == sf::Event::Closed)
			members.manager.quit();
}

void EndState::draw()
{
	members.window.clear(sf::Color(77, 77, 77));

	members.window.draw(wonText);

	members.window.display();
}
