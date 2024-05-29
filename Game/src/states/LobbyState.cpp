#include "LobbyState.hpp"
#include "GameState.hpp"
#include "protocol.hpp"
#include <iostream>

LobbyState::LobbyState(Members& members, std::string ip)
	: members(members), isFocused(true), ip(ip)
{
	members.tcpSocket.setBlocking(false);
	
	lobbyText.setFont(members.font);
	lobbyText.setString("Lobby");
	lobbyText.setCharacterSize(70);
	lobbyText.setFillColor(sf::Color::Magenta);
	lobbyText.setPosition(0, 0);
	lobbyText.setStyle(sf::Text::Bold);

	statusText.setFont(members.font);
	statusText.setFillColor(sf::Color::Green);
	statusText.setString("Waiting for players...");
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
		else if (event.type == sf::Event::GainedFocus)
			isFocused = true;
		else if (event.type == sf::Event::LostFocus)
			isFocused = false;
	}

	try
	{
		auto [key, value] = protocol::receiveKeyValue(members.tcpSocket);
		if (key == "player") // value is new player name
		{
			sf::Text text;
			text.setFont(members.font);
			text.setCharacterSize(30);
			text.setPosition(0, lobbyText.getGlobalBounds().height + playerNamesTexts.size() * 30 + 20);
			text.setString(value);
			playerNamesTexts.push_back(text);
		}
		else if (key == "index") // value is the index of the player
			members.playerIndex = std::stoi(value);
		else if (key == "start") // no value
		{
			std::unique_ptr<GameState> gameState = std::make_unique<GameState>(members, isFocused, ip);
			members.manager.setState(std::move(gameState));
		}
		else if (key == "soon") // no value
			statusText.setString("Starting!");
	}
	catch (sockets::exception& err)
	{
		std::cout << err.what() << std::endl;
	}
}

void LobbyState::draw()
{
	members.window.clear(sf::Color(77, 77, 77));

	members.window.draw(lobbyText);

	for (auto& text : playerNamesTexts)
	{
		members.window.draw(text);
	}

	statusText.setOrigin(0, statusText.getCharacterSize());
	statusText.setPosition(0, members.window.getSize().y - 10);
	members.window.draw(statusText);

	members.window.display();
}
