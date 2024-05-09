#include "util.hpp"
#include <math.h>
#include <iostream>

float degToRad(float degrees) {
	return degrees * (M_PI / 180);
}

float vecMagnitude(const sf::Vector2f& vec) {
	return sqrtf(vec.x * vec.x + vec.y * vec.y);
}
float vecAngle(const sf::Vector2f& vec) {
	return atan2f(vec.y, vec.x);
}
sf::Vector2f vecNormalize(const sf::Vector2f& vec) {
	if (vec == sf::Vector2f(0, 0))
		return vec;
	float magnitude = vecMagnitude(vec);
	return { vec.x / magnitude, vec.y / magnitude };
}


sf::Vector2f operator+(const sf::Vector2f& vec1, const sf::Vector2f& vec2) {
	return { vec1.x + vec2.x, vec1.y + vec2.y };
}
sf::Vector2f operator-(const sf::Vector2f& vec1, const sf::Vector2f& vec2) {
	return { vec1.x - vec2.x, vec1.y - vec2.y };
}
sf::Vector2f operator*(const sf::Vector2f& vec, float a) {
	return { vec.x * a, vec.y * a };
}
sf::Vector2f operator*(float a, const sf::Vector2f& vec) {
	return { vec.x * a, vec.y * a };
}
sf::Vector2f operator/(const sf::Vector2f& vec, float a) {
	return { vec.x / a, vec.y / a };
}


std::tuple<bool, std::string, std::string> receiveKeyValue(sockets::Socket& socket) {
	std::string key = "", value = "";
	std::string data;
	bool toKey = true;

	while (data != "\n") {
		try {
			data = socket.recvString(1);

			if (data == ":")
				toKey = false;
			else if (data != "\n") {
				if (toKey)
					key += data;
				else
					value += data;
			}
		}
		catch (std::exception& err) {
			if (err.what() == std::to_string(WSAEWOULDBLOCK))
				return std::make_tuple(false, "", "");
			std::cout << "Error in recv: " << err.what() << std::endl;
		}
	}

	return std::make_tuple(true, key, value);
}
