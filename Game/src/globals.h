#pragma once

#include <array>

// global constants
namespace consts {
	// maze size
	const int MAZE_WIDTH = 16;
	const int MAZE_HEIGHT = 16;

	// the world array size (with walls)
	const int WORLD_WIDTH = MAZE_WIDTH * 2 + 1;
	const int WORLD_HEIGHT = MAZE_HEIGHT * 2 + 1;

	// cell state
	enum
	{
		CELL_EMPTY =	0,
		CELL_WALL =		1,
		CELL_VISITED =	2
	};
}

using MazeArr = std::array<std::array<char, consts::WORLD_WIDTH>, consts::WORLD_HEIGHT>;
