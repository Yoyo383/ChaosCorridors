#pragma once

#include "SFML/System.hpp"
#include "sockets.hpp"

float degToRad(float degrees);

float vecMagnitude(const sf::Vector2f& vec);
float vecAngle(const sf::Vector2f& vec);
sf::Vector2f vecNormalize(const sf::Vector2f& vec);

sf::Vector2f operator+(const sf::Vector2f& vec1, const sf::Vector2f& vec2);
sf::Vector2f operator-(const sf::Vector2f& vec1, const sf::Vector2f& vec2);
sf::Vector2f operator*(const sf::Vector2f& vec, float a);
sf::Vector2f operator*(float a, const sf::Vector2f& vec);
sf::Vector2f operator/(const sf::Vector2f& vec, float a);
