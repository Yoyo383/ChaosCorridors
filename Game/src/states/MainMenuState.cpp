#include "MainMenuState.hpp"
#include <iostream>
#include "LobbyState.hpp"
#include "sockets.hpp"
#include "protocol.hpp"
#include <future>

using namespace std::chrono_literals;

MainMenuState::MainMenuState(Members& members)
	: members(members),
	hostButton({ members.window.getSize().x / 2.0f, members.window.getSize().y / 2.0f }, members.textures, "playButton", "playButtonPressed"),
	nameField({ 100, 100 }, members.font),
	ipField({ 100, 150 }, members.font),
	connectThread{},
	canConnect(true)
{
	hostButton.setSizeRelativeToWindow(members.window, 0.5f);
	errorText.setFont(members.font);
	errorText.setFillColor(sf::Color::Red);

	background.setTexture(members.textures["ceiling"]);
	background.setScale(2, 2);
	background.setTextureRect(sf::IntRect({ 0, 0 }, { (int)members.window.getSize().x, (int)members.window.getSize().y }));
	//float scale = (float)members.window.getSize().x / members.textures["ceiling"].getSize().x;
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
			ipField.setFocus(ipField.contains(pos));

			if (hostButton.isButtonClicked(pos))
			{
				hostButton.setClicked(false);
				try
				{
					std::string ip = ipField.getText();

					if (nameField.getText() == "")
					{
						errorText.setString("Please enter a name.");
						return;
					}

					members.tcpSocket.connect({ ip, globals::TCP_PORT });

					members.tcpSocket.send(protocol::keyValueMessage("player", nameField.getText()));

					// get available port
					members.udpSocket.bind({ "0.0.0.0", 0 });
					sockets::Address udpAddress = members.udpSocket.getSocketName();

					// send UDP port
					members.tcpSocket.send(protocol::keyValueMessage("udp", std::to_string(udpAddress.port)));

					std::unique_ptr<LobbyState> lobbyState = std::make_unique<LobbyState>(members, ip);
					members.manager.addState(std::move(lobbyState));
				}
				catch (sockets::exception& err)
				{
					errorText.setString("Can't connect to server.");
					std::cout << err.what() << std::endl;
					if (std::string(err.what()) == "Socket timed out")
						members.tcpSocket = sockets::Socket(sockets::Protocol::TCP);
				}
			}
		}
		else
		{
			nameField.handleInput(event);
			ipField.handleInput(event);
		}
	}
}

void MainMenuState::draw()
{
	members.window.clear(sf::Color(77, 77, 77));
	//members.window.draw(background);

	hostButton.draw(members.window);
	nameField.draw(members.window);
	ipField.draw(members.window);

	errorText.setOrigin(0, errorText.getCharacterSize());
	errorText.setPosition(0, members.window.getSize().y - 10);
	members.window.draw(errorText);

	members.window.display();
}
