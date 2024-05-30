#pragma once

#include <array>

// global constants
namespace globals
{
	inline const unsigned short TCP_PORT = 23456;
	inline const unsigned short UDP_PORT = 23456;

	inline const int MAX_LIFE = 3;

	// maze size
	inline const int MAZE_WIDTH = 8;
	inline const int MAZE_HEIGHT = 8;

	// the world array size (with walls)
	inline const int WORLD_WIDTH = MAZE_WIDTH * 2 + 1;
	inline const int WORLD_HEIGHT = MAZE_HEIGHT * 2 + 1;

	// how many seconds is the game
	inline const int GAME_TIME = 120;

	// cell state
	enum
	{
		CELL_EMPTY = 0,
		CELL_WALL = 1,
		CELL_VISITED = 2
	};

	// type for the maze array (2d array of bytes)
	using MazeArr = std::array<std::array<char, WORLD_WIDTH>, WORLD_HEIGHT>;
}
