#include "maze.hpp"
#include <stack>
#include <vector>

struct Cell {
	int x, y;
};

MazeArr generateMaze() {
	std::stack<Cell> stack;
	MazeArr maze = MazeArr();
	int visitedCells = 0;

	// reset the maze
	for (int i = 0; i < consts::WORLD_HEIGHT; i++) {
		for (int j = 0; j < consts::WORLD_WIDTH; j++) {
			if (i % 2 == 0 || j % 2 == 0)
				maze[i][j] = consts::CELL_WALL;
			else
				maze[i][j] = consts::CELL_EMPTY;
		}
	}

	// first cell
	maze[1][1] = consts::CELL_VISITED;
	stack.push({ 1, 1 });
	visitedCells = 1;

	// loop until every cell is visited
	while (visitedCells < consts::MAZE_HEIGHT * consts::MAZE_WIDTH) {
		std::vector<int> neighbors;
		Cell top = stack.top();
		maze[top.y][top.x] = consts::CELL_VISITED;

		// get available neighbors
		if (top.y > 1 && maze[top.y - 2][top.x] != consts::CELL_VISITED) // north
			neighbors.push_back(0);
		if (top.y < consts::WORLD_HEIGHT - 2 && maze[top.y + 2][top.x] != consts::CELL_VISITED) // south
			neighbors.push_back(1);
		if (top.x > 1 && maze[top.y][top.x - 2] != consts::CELL_VISITED) // west
			neighbors.push_back(2);
		if (top.x < consts::WORLD_WIDTH - 2 && maze[top.y][top.x + 2] != consts::CELL_VISITED) // east
			neighbors.push_back(3);

		if (!neighbors.empty()) {
			int nextCellDir = neighbors[rand() % neighbors.size()];

			// delete the wall of the chosen neighbor
			switch (nextCellDir) {
			case 0: // north
				maze[top.y - 1][top.x] = consts::CELL_EMPTY;
				stack.push({ top.x, top.y - 2 });
				break;
				
			case 1: // south
				maze[top.y + 1][top.x] = consts::CELL_EMPTY;
				stack.push({ top.x, top.y + 2 });
				break;

			case 2: // west
				maze[top.y][top.x - 1] = consts::CELL_EMPTY;
				stack.push({ top.x - 2, top.y });
				break;

			case 3: // east
				maze[top.y][top.x + 1] = consts::CELL_EMPTY;
				stack.push({ top.x + 2, top.y });
				break;
			}

			visitedCells++;
		}
		else
		{
			// go back
			stack.pop();
		}
	}

	// set all visited cells to 0 for ease of use
	for (int i = 0; i < consts::WORLD_HEIGHT; i++) {
		for (int j = 0; j < consts::WORLD_WIDTH; j++) {
			if (maze[i][j] == consts::CELL_VISITED)
				maze[i][j] = consts::CELL_EMPTY;
		}
	}

	// make an exit
	maze[consts::WORLD_HEIGHT - 2][consts::WORLD_WIDTH - 1] = consts::CELL_EMPTY;

	return maze;
}

