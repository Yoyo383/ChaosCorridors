#pragma once

// global constants
namespace consts {
	// maze size
	const int MAZE_WIDTH = 16;
	const int MAZE_HEIGHT = 16;

	// the world array size (with walls)
	const int WORLD_WIDTH = MAZE_WIDTH * 2 + 1;
	const int WORLD_HEIGHT = MAZE_HEIGHT * 2 + 1;
}
