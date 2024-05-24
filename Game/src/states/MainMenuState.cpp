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
	canConnect(true),
	doesFutureExist(false)
{
	hostButton.setSizeRelativeToWindow(members.window, 0.5f);
	errorText.setFont(members.font);
	errorText.setFillColor(sf::Color::Red);
}

bool MainMenuState::connectToServer()
{
	errorText.setFillColor(sf::Color::Yellow);
	errorText.setString("Connecting to server...");
	try
	{
		members.tcpSocket.connect({ ip, globals::TCP_PORT });
	}
	catch (sockets::exception& err)
	{
		errorText.setFillColor(sf::Color::Red);
		errorText.setString("Can't connect to server.");

		std::cout << err.what() << std::endl;
		if (std::string(err.what()) == "Socket timed out")
			members.tcpSocket = sockets::Socket(sockets::Protocol::TCP);

		return false;
	}
	return true;
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
				ip = ipField.getText();

				if (nameField.getText() == "")
				{
					errorText.setString("Please enter a name.");
					return;
				}

				if (canConnect)
				{
					connectFuture = std::async(std::launch::async, &MainMenuState::connectToServer, this);
					canConnect = false;
					doesFutureExist = true;
				}
			}
		}
		else
		{
			nameField.handleInput(event);
			ipField.handleInput(event);
		}
	}

	if (!doesFutureExist || connectFuture.wait_for(0s) != std::future_status::ready)
		return;

	bool connected = connectFuture.get();
	if (!connected)
	{
		canConnect = true;
		doesFutureExist = false;
		return;
	}

	try
	{
		members.tcpSocket.send(protocol::keyValueMessage("player", nameField.getText()));

		// get available port
		members.udpSocket.bind({ "0.0.0.0", 0 });
		sockets::Address udpAddress = members.udpSocket.getSocketName();

		// send UDP port
		members.tcpSocket.send(protocol::keyValueMessage("udp", std::to_string(udpAddress.port)));

		std::unique_ptr<LobbyState> lobbyState = std::make_unique<LobbyState>(members, ip);
		members.manager.setState(std::move(lobbyState));
	}
	catch (sockets::exception& err)
	{
		errorText.setFillColor(sf::Color::Red);
		errorText.setString("Can't start connection with server.");
		std::cout << err.what() << std::endl;
	}
}

void MainMenuState::draw()
{
	members.window.clear(sf::Color(77, 77, 77));

	hostButton.draw(members.window);
	nameField.draw(members.window);
	ipField.draw(members.window);

	errorText.setOrigin(0, errorText.getCharacterSize());
	errorText.setPosition(0, members.window.getSize().y - 10);
	members.window.draw(errorText);

	members.window.display();
}
