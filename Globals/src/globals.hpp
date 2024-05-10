#pragma once

#include <array>

// global constants
namespace globals {
	// maze size
	const int MAZE_WIDTH = 4;
	const int MAZE_HEIGHT = 4;

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

	// type for the maze array (2d array of bytes)
	using MazeArr = std::array<std::array<char, WORLD_WIDTH>, WORLD_HEIGHT>;
}
