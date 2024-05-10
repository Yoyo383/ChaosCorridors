#include "graphics.hpp"
#include "util.hpp"
#include <iostream>

bool TextureManager::addTexture(std::string id, std::string filename) {
	if (map.find(id) != map.end()) {
		std::cout << "Texture with the ID of '" << id << "' already exists." << std::endl;;
		return false;
	}
			
	sf::Texture texture;
	if (!texture.loadFromFile(filename))
		throw std::exception(("ERROR: Can't load " + filename).c_str());

	map[id] = texture;
	return true;
}

sf::Texture& TextureManager::operator[](std::string id) {
	if (map.find(id) == map.end())
		throw std::exception(("ERROR: No texture with the ID of '" + id + "'.").c_str());
	return map[id];
}

Ray raycast(sf::Vector2f pos, float angle, const globals::MazeArr& world) {
	// the result
	sf::Vector2f hit(-1, -1);

	// ray direction
	sf::Vector2f rayDir = { cosf(angle), sinf(angle) };

	// the unit step size
	sf::Vector2f rayUnitStepSize = {
		sqrt(1 + (rayDir.y / rayDir.x) * (rayDir.y / rayDir.x)),
		sqrt(1 + (rayDir.x / rayDir.y) * (rayDir.x / rayDir.y))
	};

	sf::Vector2i currentCell = { (int)pos.x, (int)pos.y };
	sf::Vector2f rayLength1D;
	sf::Vector2i step;

	// set step and initial ray length
	if (rayDir.x < 0) {
		step.x = -1;
		rayLength1D.x = (pos.x - float(currentCell.x)) * rayUnitStepSize.x;
	}
	else {
		step.x = 1;
		rayLength1D.x = (float(currentCell.x + 1) - pos.x) * rayUnitStepSize.x;
	}

	if (rayDir.y < 0) {
		step.y = -1;
		rayLength1D.y = (pos.y - float(currentCell.y)) * rayUnitStepSize.y;
	}
	else {
		step.y = 1;
		rayLength1D.y = (float(currentCell.y + 1) - pos.y) * rayUnitStepSize.y;
	}

	bool foundCell = false;
	bool verticalHit = false;
	float maxDistance = globals::WORLD_WIDTH;
	float distance = 0;
	float hitCoord = 0;

	// walk on the ray until collision (or distance is bigger than maxDistance)
	while (!foundCell && distance < maxDistance) {
		if (rayLength1D.x < rayLength1D.y) {
			currentCell.x += step.x;
			distance = rayLength1D.x;
			rayLength1D.x += rayUnitStepSize.x;
			verticalHit = true;
		}
		else {
			currentCell.y += step.y;
			distance = rayLength1D.y;
			rayLength1D.y += rayUnitStepSize.y;
			verticalHit = false;
		}

		if (currentCell.x >= 0 && currentCell.x < globals::WORLD_WIDTH && currentCell.y >= 0 && currentCell.y < globals::WORLD_HEIGHT)
		{
			if (world[currentCell.y][currentCell.x] == globals::CELL_WALL)
			{
				foundCell = true;
			}
		}
	}

	sf::Vector2f hitPos = pos + rayDir * distance;

	if (verticalHit)
		hitCoord = hitPos.y;
	else
		hitCoord = hitPos.x;

	return { foundCell, verticalHit, distance, hitCoord - int(hitCoord) };
}
