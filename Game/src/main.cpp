#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

sf::Vector2f raycast(sf::Vector2f pos) {

	return sf::Vector2f(-1, -1);
}

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 800), "Yay window!", sf::Style::Titlebar | sf::Style::Close);

	int world[8][8] = { {0, 0, 0, 0, 0, 0, 0, 0},
						{0, 1, 1, 0, 0, 0, 0, 1},
						{1, 1, 1, 0, 1, 0, 1, 1},
						{0, 1, 1, 0, 1, 0, 0, 1},
						{0, 0, 0, 0, 1, 0, 0, 1},
						{1, 1, 1, 0, 0, 0, 0, 0},
						{1, 1, 1, 0, 0, 0, 1, 1},
						{0, 0, 0, 0, 0, 1, 1, 1} };

	sf::Clock delta_clock;
	float dt;

	sf::Vector2f pos(0.5, 0.5);
	float speed = 0.5;

	sf::Vector2f hit;

	sf::CircleShape circle(10);
	circle.setFillColor(sf::Color::Green);
	circle.setOrigin(10, 10);
	circle.setPosition(pos);

	while (window.isOpen()) {
		dt = delta_clock.restart().asSeconds();
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			window.close();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			pos.y -= speed * dt;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			pos.y += speed * dt;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			pos.x -= speed * dt;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			pos.x += speed * dt;


		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			hit = raycast(pos);


		window.clear(sf::Color::Black);


		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				int value = world[i][j];
				sf::Color c;
				value == 0 ? c = sf::Color::White : c = sf::Color::Blue;
				sf::RectangleShape square(sf::Vector2f(100, 100));
				square.setFillColor(c);
				square.setPosition(j * 100, i * 100);
				window.draw(square);
			}
		}


		circle.setPosition(pos.x * 800, pos.y * 800);
		window.draw(circle);

		window.display();
	}
}
