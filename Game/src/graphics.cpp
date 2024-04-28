#include "graphics.hpp"
#include "util.hpp"

namespace graphics {

	Textures loadTextures() {
		Textures textures;
		if (!textures.wallTexture.loadFromFile("assets/redbrick.png"))
			throw std::exception("ERROR: Can't load wall texture.");

		if (!textures.floorTexture.loadFromFile("assets/wood.png"))
			throw std::exception("ERROR: Can't load floor texture.");
		textures.floorTexture.setRepeated(true);

		if (!textures.ceilingTexture.loadFromFile("assets/colorstone.png"))
			throw std::exception("ERROR: Can't load ceiling texture.");
		textures.ceilingTexture.setRepeated(true);

		if (!textures.characterTexture.loadFromFile("assets/character.png"))
			throw std::exception("ERROR: Can't load character texture.");

		return textures;
	}

	Ray raycast(sf::Vector2f pos, float angle, const MazeArr& world) {
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
		float maxDistance = consts::WORLD_WIDTH;
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

			if (currentCell.x >= 0 && currentCell.x < consts::WORLD_WIDTH && currentCell.y >= 0 && currentCell.y < consts::WORLD_HEIGHT)
			{
				if (world[currentCell.y][currentCell.x] == consts::CELL_WALL)
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

	void drawFloorAndCeiling(sf::RenderWindow& window, const Player& player, Textures textures) {
		// scale factor for d (and other stuff)
		float tan = tanf(player.fov() / 2);

		// for doing floor/ceiling things
		float cos = cosf(player.direction()), sin = sinf(player.direction());

		for (int y = 0; y <= window.getSize().y / 2; y++) {
			// the distance of the current row
			// floor and ceiling texture have the same size so it doesn't matter which size is in the formula
			int d = textures.floorTexture.getSize().x * window.getSize().y / 2 / (y + 1);

			sf::Vector2f startPos = {
				d * cos - d * tan * sin,
				d * sin + d * tan * cos
			};
			sf::Vector2f endPos = {
				d * cos + d * tan * sin,
				d * sin - d * tan * cos
			};

			// calculating shading based on distance
			sf::Color color = sf::Color::White;
			float brightness = 1.0f - (float)d / window.getSize().y;
			if (brightness < 0)
				brightness = 0;
			color.r *= brightness;
			color.g *= brightness;
			color.b *= brightness;


			sf::VertexArray scanLine(sf::Lines, 2);

			// texturing the floor/ceiling according to the start and end sample positions and the player position
			scanLine[0].texCoords = endPos + player.pos() * textures.floorTexture.getSize().x;
			scanLine[1].texCoords = startPos + player.pos() * textures.floorTexture.getSize().x;
			// shading
			scanLine[0].color = color;
			scanLine[1].color = color;


			// setting floor position
			scanLine[0].position = { 0, (float)y + window.getSize().y / 2 };
			scanLine[1].position = { (float)window.getSize().x, (float)y + window.getSize().y / 2 };

			window.draw(scanLine, &textures.floorTexture);


			// setting ceiling position
			scanLine[0].position = { 0, window.getSize().y / 2 - (float)y };
			scanLine[1].position = { (float)window.getSize().x, window.getSize().y / 2 - (float)y };

			window.draw(scanLine, &textures.ceilingTexture);
		}
	}

	void drawWalls(sf::RenderWindow& window, const Player& player, const MazeArr& maze, float* zBuffer, Textures textures) {
		// variables for angle increment
		// math taken from here:
		// https://stackoverflow.com/questions/24173966/raycasting-engine-rendering-creating-slight-distortion-increasing-towards-edges
		float screenHalfLen = tanf(player.fov() / 2);
		float segLen = 2 * screenHalfLen / window.getSize().x;

		float angle;
		for (int x = 0; x <= window.getSize().x; x++) {
			// angle calculation such that the walls aren't distorted
			angle = player.direction() + atanf(segLen * x - screenHalfLen);

			// casting ray and fixing the fisheye problem
			Ray ray = raycast(player.pos(), angle, maze);
			ray.distance *= cosf(player.direction() - angle);

			zBuffer[x] = ray.distance;

			if (!ray.isHit)
				continue;

			float wallHeight = (float)window.getSize().y / ray.distance;

			// calculating floor and ceiling y values
			float ceiling = (window.getSize().y - wallHeight) / 2.0f;
			float floor = window.getSize().y - ceiling;

			// calculating shading
			sf::Color color = sf::Color::White;
			float brightness = 1.0f - (ray.distance / 16);

			// darkening the vertical walls
			if (ray.verticalHit)
				brightness *= 0.7;

			if (brightness < 0)
				brightness = 0;

			// apply brightness
			color.r *= brightness;
			color.g *= brightness;
			color.b *= brightness;

			// making sure the texture is the right aspect ration
			float xMultiplier = (float)window.getSize().x / window.getSize().y;
			// the x coord to sample from in the texture
			float textureX = (int)(textures.wallTexture.getSize().x * ray.hitCoord * xMultiplier) % textures.wallTexture.getSize().x;


			sf::VertexArray wall(sf::Lines, 2);

			// setting wall position and height
			wall[0].position = { (float)x, ceiling };
			wall[1].position = { (float)x, floor };

			// shading the wall
			wall[0].color = color;
			wall[1].color = color;

			// texturing the wall according to the hit coordinate
			wall[0].texCoords = { textureX, 0 };
			wall[1].texCoords = { textureX, (float)textures.wallTexture.getSize().y };

			window.draw(wall, &textures.wallTexture);
		}
	}

	void drawCharacter(sf::RenderWindow& window, const Player& player, const sf::Vector2f& characterPos, float* zBuffer, Textures textures) {
		float angleFromPlayer = vecAngle(characterPos - player.pos());
		float relativeAngle = player.direction() - angleFromPlayer;

		if (relativeAngle > M_PI)
			relativeAngle -= 2 * M_PI;
		else if (relativeAngle < -M_PI)
			relativeAngle += 2 * M_PI;

		float distance = vecMagnitude(characterPos - player.pos());
		distance *= cosf(relativeAngle);

		if (distance >= 0.5f) {
			float height = (float)window.getSize().y / distance;

			// calculating floor and ceiling y values
			float ceiling = (window.getSize().y - height) / 2.0f;
			float floor = window.getSize().y - ceiling;

			// getting width of texture
			float aspectRatio = (float)textures.characterTexture.getSize().x / textures.characterTexture.getSize().y;
			float width = height * aspectRatio;

			// calculating middle of texture
			float middle = window.getSize().x - (relativeAngle / player.fov() + 0.5f) * window.getSize().x;

			for (int x = 0; x <= width; x++) {
				float posX = middle + (float)x - (width / 2.0f);

				// if outside window or behind walls then don't draw
				if (posX < 0 || posX > window.getSize().x || zBuffer[(int)posX] < distance)
					continue;

				sf::VertexArray character(sf::Lines, 2);

				sf::Color color = sf::Color::White;
				float brightness = 1.0f - (distance / 16);

				if (brightness < 0)
					brightness = 0;

				// apply brightness
				color.r *= brightness;
				color.g *= brightness;
				color.b *= brightness;

				// setting position and height
				character[0].position = { posX, ceiling };
				character[1].position = { posX, floor };

				// shading
				character[0].color = color;
				character[1].color = color;

				// texturing
				float textureX = textures.characterTexture.getSize().x * (float)x / width;
				character[0].texCoords = { textureX, 0 };
				character[1].texCoords = { textureX, (float)textures.characterTexture.getSize().y };

				window.draw(character, &textures.characterTexture);
			}
		}
	}

}
