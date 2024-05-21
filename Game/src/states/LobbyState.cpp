#include "LobbyState.hpp"
#include "GameState.hpp"
#include "protocol.hpp"
#include <iostream>

LobbyState::LobbyState(Members& members)
	: members(members)
{
	font.loadFromFile("C:/Windows/Fonts/Arial.ttf");
	members.tcpSocket.setBlocking(false);
}

void LobbyState::update()
{
	sf::Event event;

	while (members.window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			members.tcpSocket.send(protocol::keyValueMessage("close", std::to_string(members.playerIndex)));
			members.tcpSocket.close();
			members.manager.quit();
			return;
		}
	}

	try
	{
		auto [key, value] = protocol::receiveKeyValue(members.tcpSocket);
		if (key == "player")
		{
			sf::Text text;
			text.setFont(font);
			text.setCharacterSize(90);
			text.setPosition(0, playerNamesTexts.size() * 90);
			text.setString(value);
			text.setFillColor(sf::Color::Black);
			playerNamesTexts.push_back(text);
		}
		else if (key == "index")
			members.playerIndex = std::stoi(value);
		else if (key == "start")
		{
			std::unique_ptr<GameState> gameState = std::make_unique<GameState>(members);
			members.manager.setState(std::move(gameState));
		}
	}
	catch (std::exception& err)
	{
		std::cout << err.what() << std::endl;
	}
}

void LobbyState::draw()
{
	members.window.clear(sf::Color::White);
	for (auto& text : playerNamesTexts)
	{
		members.window.draw(text);
	}
	members.window.display();
}
