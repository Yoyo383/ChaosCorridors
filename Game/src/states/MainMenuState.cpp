#include "MainMenuState.hpp"
#include <iostream>
#include "LobbyState.hpp"
#include "sockets.hpp"
#include "protocol.hpp"

MainMenuState::MainMenuState(Members& members)
	: members(members),
	hostButton({ members.window.getSize().x / 2.0f, members.window.getSize().y / 2.0f }, members.textures, "hostButton", "buttonNormal"),
	nameField({ 100, 100 }, members.font)
{
	hostButton.setSizeRelativeToWindow(members.window, 0.5f);
}

void MainMenuState::update()
{
	sf::Event event;

	while (members.window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			members.manager.quit();
		else if (event.type == sf::Event::MouseButtonPressed)
			hostButton.isButtonClicked(sf::Vector2f(sf::Mouse::getPosition(members.window)));

		else if (event.type == sf::Event::MouseButtonReleased)
		{
			sf::Vector2f pos = sf::Vector2f(sf::Mouse::getPosition(members.window));
			nameField.setFocus(nameField.contains(pos));

			if (hostButton.isButtonClicked(pos))
			{
				hostButton.setClicked(false);
				try
				{
					members.tcpSocket.connect({ "127.0.0.1", 12345 });
					members.tcpSocket.send(protocol::keyValueMessage("player", nameField.getText()));

					// get available port
					members.udpSocket.bind({ "0.0.0.0", 0 });
					sockets::Address udpAddress = members.udpSocket.getSocketName();

					// send UDP port
					members.tcpSocket.send(protocol::keyValueMessage("udp", std::to_string(udpAddress.port)));

					std::unique_ptr<LobbyState> lobbyState = std::make_unique<LobbyState>(members);
					members.manager.addState(std::move(lobbyState));
				}
				catch (std::exception& err)
				{
					std::cout << "Can't connect to server." << std::endl;
				}
			}
		}
		else
		{
			nameField.handleInput(event);
		}
	}
}

void MainMenuState::draw()
{
	members.window.clear(sf::Color::White);

	hostButton.draw(members.window);
	nameField.draw(members.window);

	members.window.display();
}
