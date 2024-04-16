#pragma once

#include <array>
#include "globals.h"

// for easier code
using MazeArr = std::array<std::array<char, consts::WORLD_WIDTH>, consts::WORLD_HEIGHT>;

MazeArr generateMaze();