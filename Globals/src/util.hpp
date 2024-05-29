#pragma once

#include "SFML/System/Vector2.hpp"

/**
 * @brief Generates a random int between min and max (both inclusive).
 * @param min Minimum value (inclusive).
 * @param max Maximum value (inclusive).
 * @return The random number generated.
 */
int randInt(int min, int max);

/**
 * @brief Turns degrees to radians.
 * @param degrees Degrees.
 * @return Equivalent radians.
 */
float degToRad(float degrees);

/**
 * @brief Calculates the magnitude (length) of a vector.
 * @param vec The vector.
 * @return The vector's magnitude.
 */
float vecMagnitude(const sf::Vector2f& vec);
/**
 * @brief Calculates the angle of a vector.
 * @param vec The vector.
 * @return The vector's angle.
 */
float vecAngle(const sf::Vector2f& vec);
/**
 * @brief Normalizes a vector (keeps the angle but with length 1).
 * @param vec The vector.
 * @return The normalized vector.
 */
sf::Vector2f vecNormalize(const sf::Vector2f& vec);

// Operators for vectors (adding, subtracting, multiplying, dividing):

sf::Vector2f operator+(const sf::Vector2f& vec1, const sf::Vector2f& vec2);
sf::Vector2f operator-(const sf::Vector2f& vec1, const sf::Vector2f& vec2);
sf::Vector2f operator*(const sf::Vector2f& vec, float a);
sf::Vector2f operator*(float a, const sf::Vector2f& vec);
sf::Vector2f operator/(const sf::Vector2f& vec, float a);
